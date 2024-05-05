#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>

// Function pointer to the LJRat CRT HeapAlloc function
// DLL Module will be linked against its own static CRT, need to ensure
// Correct Heap is utilized
typedef PVOID (*PHEAPALLOCFUNC)(HANDLE, DWORD, size_t);


typedef struct common_t
{
    PHEAPALLOCFUNC pHeapAlloc;
    PVOID pData;
} COMMON, *PCOMMON

#endif