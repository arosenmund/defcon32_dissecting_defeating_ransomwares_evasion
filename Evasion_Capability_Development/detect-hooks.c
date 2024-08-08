#include <iostream>
#include <Windows.h>
#include <psapi.h>

int main()
{
	PDWORD functionAddress = (PDWORD)0;
	
	// Get ntdll base address
	HMODULE libraryBase = LoadLibraryA("ntdll");

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)libraryBase;
	PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)libraryBase + dosHeader->e_lfanew);

	// Locate export address table
	DWORD_PTR exportDirectoryRVA = imageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	PIMAGE_EXPORT_DIRECTORY imageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)libraryBase + exportDirectoryRVA);

	// Offsets to list of exported functions and their names
	PDWORD addresOfFunctionsRVA = (PDWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfFunctions);
	PDWORD addressOfNamesRVA = (PDWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfNames);
	PWORD addressOfNameOrdinalsRVA = (PWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfNameOrdinals);

	// Iterate through exported functions of ntdll
	for (DWORD i = 0; i < imageExportDirectory->NumberOfNames; i++)
	{
		// Resolve exported function name
		DWORD functionNameRVA = addressOfNamesRVA[i];
		DWORD_PTR functionNameVA = (DWORD_PTR)libraryBase + functionNameRVA;
		char* functionName = (char*)functionNameVA;
		
		// Resolve exported function address
		DWORD_PTR functionAddressRVA = 0;
		functionAddressRVA = addresOfFunctionsRVA[addressOfNameOrdinalsRVA[i]];
		functionAddress = (PDWORD)((DWORD_PTR)libraryBase + functionAddressRVA);

		// Syscall stubs start with these bytes
		unsigned char syscallPrologue[4] = { 0x4c, 0x8b, 0xd1, 0xb8 };

		// Only interested in Nt|Zw functions
		if (strncmp(functionName, (char*)"Nt", 2) == 0 || strncmp(functionName, (char*)"Zw", 2) == 0)
		{
			// Check if the first 4 instructions of the exported function are the same as the sycall's prologue
			if (memcmp(functionAddress, syscallPrologue, 4) != 0) {
			
				if (*((unsigned char*)functionAddress) == 0xE9) // first byte is a jmp instruction, where does it jump to?
				{
					DWORD jumpTargetRelative = *((PDWORD)((char*)functionAddress + 1));
					PDWORD jumpTarget = functionAddress + 5 /*Instruction pointer after our jmp instruction*/ + jumpTargetRelative;  
					char moduleNameBuffer[512];
					GetMappedFileNameA(GetCurrentProcess(), jumpTarget, moduleNameBuffer, 512);
					
					printf("Hooked: %s : %p into module %s\n", functionName, functionAddress, moduleNameBuffer);
				}
				else
				{
					printf("Potentially hooked: %s : %p\n", functionName, functionAddress);
				}
			
			
				
			}
		}
	}

	return 0;
}
