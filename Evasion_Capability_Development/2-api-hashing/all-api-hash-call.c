#include <iostream>
#include <windows.h>
#include <DbgHelp.h>
#include <TlHelp32.h>
#include <string.h>

DWORD getHashFromString(char *string) {
    size_t stringLength = strnlen_s(string, 50);
    DWORD hash = 0x35;
    
    for (size_t i = 0; i < stringLength; i++) {
        hash += (hash * 0xab10f29f + string[i]) & 0xffffff;
    }
    
    return hash;
}

PDWORD getFunctionAddressByHash(char *library, DWORD hash) {
    PDWORD functionAddress = (PDWORD)0;

    // Get base address of the module in which our exported function of interest resides (e.g., kernel32.dll)
    HMODULE libraryBase = LoadLibraryA(library);
    if (!libraryBase) {
        printf("LoadLibraryA failed for %s with error %lu\n", library, GetLastError());
        return nullptr;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)libraryBase;
    PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)libraryBase + dosHeader->e_lfanew);
    
    DWORD_PTR exportDirectoryRVA = imageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    
    PIMAGE_EXPORT_DIRECTORY imageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)libraryBase + exportDirectoryRVA);
    
    // Get RVAs to exported function related information
    PDWORD addresOfFunctionsRVA = (PDWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfFunctions);
    PDWORD addressOfNamesRVA = (PDWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfNames);
    PWORD addressOfNameOrdinalsRVA = (PWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfNameOrdinals);

    // Iterate through exported functions, calculate their hashes and check if any of them match our hash
    for (DWORD i = 0; i < imageExportDirectory->NumberOfNames; i++) {
        DWORD functionNameRVA = addressOfNamesRVA[i];
        DWORD_PTR functionNameVA = (DWORD_PTR)libraryBase + functionNameRVA;
        char* functionName = (char*)functionNameVA;

        // Calculate hash for this exported function
        DWORD functionNameHash = getHashFromString(functionName);
        
        // If hash matches, resolve the function address
        if (functionNameHash == hash) {
            DWORD_PTR functionAddressRVA = addresOfFunctionsRVA[addressOfNameOrdinalsRVA[i]];
            functionAddress = (PDWORD)((DWORD_PTR)libraryBase + functionAddressRVA);
            printf("%s : 0x%x : %p\n", functionName, functionNameHash, functionAddress);
            return functionAddress;
        }
    }

    return nullptr;
}

// Define function prototypes using typedefs
typedef HANDLE (WINAPI *PFN_CreateThread)(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
);

typedef HANDLE (WINAPI *PFN_CreateRemoteThread)(
    HANDLE hProcess,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
);

typedef LPVOID (WINAPI *PFN_VirtualAlloc)(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD flAllocationType,
    DWORD flProtect
);

typedef BOOL (WINAPI *PFN_IsDebuggerPresent)(void);

typedef HINSTANCE (WINAPI *PFN_ShellExecute)(
    HWND hwnd,
    LPCSTR lpOperation,
    LPCSTR lpFile,
    LPCSTR lpParameters,
    LPCSTR lpDirectory,
    INT nShowCmd
);

int main() {
    // Define the hashes for the required functions
    DWORD hash_CreateThread = 0x00544e304;        // Hash for "CreateThread"
    DWORD hash_CreateRemoteThread = 0xB0FA6C52;   // Hash for "CreateRemoteThread"
    DWORD hash_VirtualAlloc = 0x91afca54;         // Hash for "VirtualAlloc"
    DWORD hash_IsDebuggerPresent = 0x82849eb9;    // Hash for "IsDebuggerPresent"
    DWORD hash_ShellExecute = 0x0cead96c;         // Hash for "ShellExecuteA"

    // Load and resolve the functions by their hashes
    PFN_CreateThread CreateThread = (PFN_CreateThread)getFunctionAddressByHash((char *)"kernel32.dll", hash_CreateThread);
    PFN_CreateRemoteThread CreateRemoteThread = (PFN_CreateRemoteThread)getFunctionAddressByHash((char *)"kernel32.dll", hash_CreateRemoteThread);
    PFN_VirtualAlloc VirtualAlloc = (PFN_VirtualAlloc)getFunctionAddressByHash((char *)"kernel32.dll", hash_VirtualAlloc);
    PFN_IsDebuggerPresent IsDebuggerPresent = (PFN_IsDebuggerPresent)getFunctionAddressByHash((char *)"kernel32.dll", hash_IsDebuggerPresent);
    PFN_ShellExecute ShellExecute = (PFN_ShellExecute)getFunctionAddressByHash((char *)"shell32.dll", hash_ShellExecute);

    // Check if all functions were resolved successfully
    if (!CreateThread || !CreateRemoteThread || !VirtualAlloc || !IsDebuggerPresent || !ShellExecute) {
        printf("Failed to resolve one or more functions.\n");
        return 1;
    }

    // Call CreateThread
    DWORD tid = 0;
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IsDebuggerPresent, NULL, 0, &tid);
    if (hThread == NULL) {
        printf("CreateThread failed with error %lu\n", GetLastError());
    } else {
        printf("CreateThread succeeded.\n");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    // Call CreateRemoteThread (for demonstration, this is called on the current process)
    HANDLE hRemoteThread = CreateRemoteThread(GetCurrentProcess(), NULL, 0, (LPTHREAD_START_ROUTINE)IsDebuggerPresent, NULL, 0, &tid);
    if (hRemoteThread == NULL) {
        printf("CreateRemoteThread failed with error %lu\n", GetLastError());
    } else {
        printf("CreateRemoteThread succeeded.\n");
        WaitForSingleObject(hRemoteThread, INFINITE);
        CloseHandle(hRemoteThread);
    }

    // Call VirtualAlloc
    LPVOID lpAddress = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpAddress == NULL) {
        printf("VirtualAlloc failed with error %lu\n", GetLastError());
    } else {
        printf("VirtualAlloc succeeded.\n");
    }

    // Call IsDebuggerPresent
    BOOL isDebuggerPresent = IsDebuggerPresent();
    printf("IsDebuggerPresent returned %d\n", isDebuggerPresent);

    // Call ShellExecute
    HINSTANCE hInstance = ShellExecute(NULL, "open", "notepad.exe", NULL, NULL, SW_SHOWNORMAL);
    if ((UINT_PTR)hInstance <= 32) {
        printf("ShellExecute failed with error %lu\n", GetLastError());
    } else {
        printf("ShellExecute succeeded.\n");
    }

    return 0;
}
