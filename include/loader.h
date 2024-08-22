#ifndef LOADER_H
#define LOADER_H

#include <Windows.h>

typedef struct
{
	WORD	offset : 12;
	WORD	type : 4;
} IMAGE_RELOC, * PIMAGE_RELOC;

typedef INT (*DLLFUNC)(VOID);


/*
@brief Function to reflectively load a dll in memory and return the exported function
@param buffer of dll
@param string of exported function to search for
@return Pointer to exported function
*/
ULONG_PTR LoadMemoryModule(PVOID pDll, PCHAR pExportedFunc);

#endif // LOADER_H