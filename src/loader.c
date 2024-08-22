#include "loader.h"

ULONG_PTR LoadMemoryModule(PVOID pDll, PCHAR pExportedFunc)
{
	ULONG_PTR pFunc = 0;
	ULONG_PTR pDllBaseAddress = 0;
	ULONG_PTR pPebAddress = 0;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionStartAddress = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDirectory = NULL;
	DWORD dwSectionCount = 0;
	ULONG_PTR pTemp = 0;

	pDosHeader = pDll;
	pNTHeader = pDosHeader->e_lfanew + (ULONG_PTR) pDll;

	// Step 1 - Allocate space in memory for the dll to live; this will be the base address of the DLL
	pDllBaseAddress = VirtualAlloc(NULL, pNTHeader->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (NULL == pDllBaseAddress)
	{
		goto end;
	}

	// Step 2 - Copy over the dll into its new memory space pDllBaseAddress
	memcpy(pDllBaseAddress, pDll, pNTHeader->OptionalHeader.SizeOfHeaders);

	dwSectionCount = pNTHeader->FileHeader.NumberOfSections;

	pSectionStartAddress = (ULONG_PTR) &pNTHeader->OptionalHeader + pNTHeader->FileHeader.SizeOfOptionalHeader;

	pTemp = pSectionStartAddress;
	DWORD dwSectionVA;
	DWORD dwRawSectionSize;
	DWORD dwRawSectionAddress;
	while (dwSectionCount != 0)
	{
		dwSectionVA = ((PIMAGE_SECTION_HEADER)pTemp)->VirtualAddress;
		dwRawSectionAddress = ((PIMAGE_SECTION_HEADER)pTemp)->PointerToRawData;
		dwRawSectionSize = ((PIMAGE_SECTION_HEADER)pTemp)->SizeOfRawData;

		memcpy(pDllBaseAddress + dwSectionVA, (ULONG_PTR) pDll + dwRawSectionAddress, dwRawSectionSize);

		pTemp += sizeof(IMAGE_SECTION_HEADER);
		dwSectionCount--;
	}

	// Step 3 - Grab the location of the import table to start resolving dependencies
	// Great references, https://0xrick.github.io/win-internals/pe6/ https://tech-zealots.com/malware-analysis/journey-towards-import-address-table-of-an-executable-file/
	pImportDirectory = pDllBaseAddress + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	pTemp = pImportDirectory;

	ULONG_PTR pOriginalFirstThunkVA;
	ULONG_PTR pFirstThunkVA;
	ULONG_PTR pImportedLibraryAddress = 0;
	ULONG_PTR pImportedLibraryNTHeader;
	ULONG_PTR pImportedLibraryExportVA;
	ULONG_PTR pImportedLibraryExportFuncArray;
	ULONG_PTR pThunkNameVA = 0;

	// import directory is an array of IMAGE_IMPORT_DESCRIPTOR and the last one will be NULL
	while (pImportDirectory->Name)
	{
		pImportedLibraryAddress = LoadLibraryA(pDllBaseAddress + pImportDirectory->Name);

		// Both OFT/FT have same data in array; once dll gets loaded the IAT holds the actual VA of function
		// OFT -> ILT
		// FT -> IAT
		pOriginalFirstThunkVA = pDllBaseAddress + pImportDirectory->OriginalFirstThunk;
		pFirstThunkVA = pDllBaseAddress + pImportDirectory->FirstThunk;

		// Array is terminated by null thunk
		while (*(PLONG_PTR)pFirstThunkVA)
		{
			// Check to see if function is exported by ordinal
			// If exported by ordinal, we look at the export table of the imported dll and index into its address of functions array
			if (( (PIMAGE_THUNK_DATA) pOriginalFirstThunkVA)->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				pImportedLibraryNTHeader = pImportedLibraryAddress + ((PIMAGE_DOS_HEADER)pImportedLibraryAddress)->e_lfanew;
				pImportedLibraryExportVA = ((PIMAGE_NT_HEADERS)pImportedLibraryNTHeader)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + pImportedLibraryAddress;
				pImportedLibraryExportFuncArray = ((PIMAGE_EXPORT_DIRECTORY)pImportedLibraryExportVA)->AddressOfFunctions + pImportedLibraryAddress;
				pImportedLibraryExportFuncArray += (IMAGE_ORDINAL(((PIMAGE_THUNK_DATA) pOriginalFirstThunkVA)->u1.Ordinal) - ((PIMAGE_EXPORT_DIRECTORY)pImportedLibraryExportVA)->Base) * sizeof(DWORD);

				*(PLONG_PTR)pFirstThunkVA = pImportedLibraryExportFuncArray;
			}
			else
			{
				// Not exported by ordinal so we grab the the function name
				pThunkNameVA = *(PLONG_PTR)pFirstThunkVA + pDllBaseAddress;
				*(PLONG_PTR)pFirstThunkVA = (ULONG_PTR)GetProcAddress(pImportedLibraryAddress, ((PIMAGE_IMPORT_BY_NAME)pThunkNameVA)->Name);
			}

			pFirstThunkVA += sizeof(ULONG_PTR);
			pOriginalFirstThunkVA += sizeof(ULONG_PTR);

		}

		(ULONG_PTR) pImportDirectory += sizeof(IMAGE_IMPORT_DESCRIPTOR);

	}

	// Step 4 - Fix up the relocations that compiler added in based off image base in pe
	ULONG_PTR pImportedLibraryRelocVA = 0;
	PIMAGE_DATA_DIRECTORY pRelocDirectory;
	PIMAGE_BASE_RELOCATION pRelocTable = NULL;
	ULONG_PTR pRelocBlockVA = 0;
	DWORD dwRelocEntryCount;

	// Calculate delta between image base and where dll was loaded
	pImportedLibraryAddress = pDllBaseAddress - pNTHeader->OptionalHeader.ImageBase;
	pRelocDirectory = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

	// Check to see if any relocations are present; Size would be 0 if there werent
	if (pRelocDirectory->Size)
	{
		// IMAGE_BASE_RELOCATION and IMAGE_RELOC data structures are contiguous in memory; next structure will be pRelocTable->SizeOfBlock away 
		pRelocTable = pRelocDirectory->VirtualAddress + pDllBaseAddress;

		while (pRelocTable->SizeOfBlock)
		{
			// Now we are looking into the first reloc block
			pRelocBlockVA = pDllBaseAddress + pRelocTable->VirtualAddress;
			dwRelocEntryCount = (pRelocTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_RELOC);

			// Now lets jump into the first relocation entry in the table
			pTemp = pRelocTable;
			(ULONG_PTR) pRelocTable += sizeof(IMAGE_BASE_RELOCATION);

			while (dwRelocEntryCount--)
			{
				if (((PIMAGE_RELOC)pRelocTable)->type == IMAGE_REL_BASED_DIR64)
				{
					*(PLONG_PTR) (pRelocBlockVA + ((PIMAGE_RELOC)pRelocTable)->offset) += pImportedLibraryAddress;
				}
				else if (((PIMAGE_RELOC)pRelocTable)->type == IMAGE_REL_BASED_HIGHLOW)
				{
					*(PDWORD)(pRelocBlockVA + ((PIMAGE_RELOC)pRelocTable)->offset) += pImportedLibraryAddress;
				}
				else if (((PIMAGE_RELOC)pRelocTable)->type == IMAGE_REL_BASED_HIGH)
				{
					*(PWORD)(pRelocBlockVA + ((PIMAGE_RELOC)pRelocTable)->offset) += HIWORD(pImportedLibraryAddress);
				}
				else if (((PIMAGE_RELOC)pRelocTable)->type == IMAGE_REL_BASED_LOW)
				{
					*(PWORD)(pRelocBlockVA + ((PIMAGE_RELOC)pRelocTable)->offset) += LOWORD(pImportedLibraryAddress);
				}

				(ULONG_PTR) pRelocTable += sizeof(IMAGE_RELOC);
			}
			
			pRelocTable = pTemp;
			(ULONG_PTR) pRelocTable = (ULONG_PTR)pRelocTable + pRelocTable->SizeOfBlock;
		}
	}


	// Step 6 - Return the requested function back to caller
	// Reference https://ferreirasc.github.io/PE-Export-Address-Table/
	PIMAGE_EXPORT_DIRECTORY pExportVA = 0;
	ULONG_PTR pExportNameTable = 0;
	ULONG_PTR pExportOrdinalTable = 0;
	ULONG_PTR pExportFunctionTable = 0;
	DWORD dwNumFuncs = 0;
	DWORD dwOrdinal = 0;
	DWORD dwCounter = 0;
	PCHAR pFuncStr = NULL;

	pExportVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + pDllBaseAddress;

	dwNumFuncs = pExportVA->NumberOfFunctions;
	pExportNameTable = pExportVA->AddressOfNames + pDllBaseAddress;
	pExportOrdinalTable = pExportVA->AddressOfNameOrdinals + pDllBaseAddress;
	pExportFunctionTable = pExportVA->AddressOfFunctions + pDllBaseAddress;


	while (dwCounter <= dwNumFuncs)
	{ 
		pFuncStr = (PCHAR) (*(PDWORD)pExportNameTable + pDllBaseAddress);

		if (0 == strcmp(pExportedFunc, pFuncStr))
		{
			dwOrdinal = *(PWORD) (pExportOrdinalTable + (dwCounter * sizeof(WORD)));
			pFunc = pDllBaseAddress + *(PDWORD)(pExportFunctionTable + (dwOrdinal * sizeof(DWORD)));
		}

		pExportNameTable += sizeof(DWORD);
		dwCounter++;
	}

end:
	return pFunc;
}
