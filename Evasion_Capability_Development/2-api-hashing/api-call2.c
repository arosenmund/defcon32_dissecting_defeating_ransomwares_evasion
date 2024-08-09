#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

// Thread function for CreateThread and CreateRemoteThread
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    printf("Thread is running...\n");
    return 0;
}

// Function to find the process ID of a running instance of notepad.exe
DWORD FindProcessId(const char* processName) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD processID = 0;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed with error %lu\n", GetLastError());
        return 0;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful.
    if (!Process32First(hProcessSnap, &pe32)) {
        printf("Process32First failed with error %lu\n", GetLastError()); // show cause of failure
        CloseHandle(hProcessSnap); // clean the snapshot object
        return 0;
    }

    // Now walk the snapshot of processes, and find the specified process.
    do {
        if (strcmp(pe32.szExeFile, processName) == 0) {
            processID = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processID;
}

int main() {
    // Find the process ID of notepad.exe
    DWORD dwProcessId = FindProcessId("notepad.exe");
    if (dwProcessId == 0) {
        printf("Could not find notepad.exe process.\n");
        return 1;
    }
    printf("Found notepad.exe with process ID %lu\n", dwProcessId);

    // 1. CreateThread
    HANDLE hThread;
    DWORD dwThreadId;
    
    hThread = CreateThread(
        NULL,             // default security attributes
        0,                // default stack size
        ThreadFunction,   // thread function name
        NULL,             // argument to thread function
        0,                // default creation flags
        &dwThreadId);     // returns the thread identifier
    
    if (hThread == NULL) {
        printf("CreateThread failed with error %lu\n", GetLastError());
    } else {
        printf("CreateThread succeeded\n");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    // 2. CreateRemoteThread
    HANDLE hProcess;
    LPVOID lpBaseAddress;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if (hProcess == NULL) {
        printf("OpenProcess failed with error %lu\n", GetLastError());
    } else {
        lpBaseAddress = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
        if (lpBaseAddress == NULL) {
            printf("VirtualAllocEx failed with error %lu\n", GetLastError());
        } else {
            hThread = CreateRemoteThread(
                hProcess,
                NULL,
                0,
                (LPTHREAD_START_ROUTINE)ThreadFunction,
                lpBaseAddress,
                0,
                &dwThreadId);

            if (hThread == NULL) {
                printf("CreateRemoteThread failed with error %lu\n", GetLastError());
            } else {
                printf("CreateRemoteThread succeeded\n");
                WaitForSingleObject(hThread, INFINITE);
                CloseHandle(hThread);
            }

            VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
        }
        CloseHandle(hProcess);
    }

    // 3. VirtualAlloc
    LPVOID lpAddress;

    lpAddress = VirtualAlloc(
        NULL,                  // system determines where to allocate the region
        4096,                  // size of the region
        MEM_COMMIT | MEM_RESERVE, // allocation type
        PAGE_READWRITE);       // memory protection

    if (lpAddress == NULL) {
        printf("VirtualAlloc failed with error %lu\n", GetLastError());
    } else {
        printf("VirtualAlloc succeeded\n");
        VirtualFree(lpAddress, 0, MEM_RELEASE);
    }

    // 4. IsDebuggerPresent
    if (IsDebuggerPresent()) {
        printf("Debugger is present\n");
    } else {
        printf("Debugger is not present\n");
    }

    // 5. ShellExecute
    HINSTANCE hInstance;
    hInstance = ShellExecute(
        NULL,           // handle to parent window
        "open",         // operation to perform
        "notepad.exe",  // path to the executable
        NULL,           // parameters
        NULL,           // default directory
        SW_SHOWNORMAL); // show window flag

    if ((UINT_PTR)hInstance <= 32) {
        printf("ShellExecute failed with error %lu\n", GetLastError());
    } else {
        printf("ShellExecute succeeded\n");
    }

    return 0;
}
