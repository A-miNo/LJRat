#include "error_code.h"
#include "message.h"
#include "debug.h"
#include <windows.h>

/*
@brief Function that executes a directory listing
@param PMESSAGE containts the parameters for the command
@param PMESSAGE is a pointer to the result that will be sent back
@return ERROR_T with status of dir
*/
ERROR_T DirCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;
    PDWORD pRemoteDirNameLen = NULL;
    PCHAR pRemoteDirName = NULL;
    WIN32_FIND_DATAA file_data = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;
	PBYTE pBuf = NULL;
	INT iFileCount = 0;
    INT i = 0;
	INT iFileDataSize = 0;
	PBYTE pTempBuf = NULL;
    DWORD dwAllocSize = 0;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pRemoteDirNameLen = (PDWORD) pMsg->pData;
    pRemoteDirName = (PCHAR) ((PBYTE) pMsg->pData + sizeof(DWORD));

    DBG_PRINT("Getting: %s:%d\n", pRemoteDirName, *pRemoteDirNameLen);

	iFileDataSize = sizeof(WIN32_FIND_DATAA);

	// Count the files so we can pre-allocate enough space for sending back
	iError = CountFiles(pRemoteDirName, &iFileCount);
	if (E_DIR_ERROR == iError)
	{
		goto end;
	}

    dwAllocSize = (iFileDataSize * iFileCount) + sizeof(DWORD);

	pBuf = (PBYTE) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAllocSize);
	if (NULL == pBuf)
	{
		iError = E_MEMORY_ERROR;
		goto end;
	}

	*(PDWORD)pBuf = iFileCount;
	pTempBuf = pBuf;
	pTempBuf = pTempBuf + sizeof(DWORD);

	hFind = FindFirstFileA(pRemoteDirName, &file_data);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		iError = E_DIR_ERROR;
		goto end;
	}

	memcpy(pTempBuf, &file_data, iFileDataSize);
	i++;

	while (FindNextFileA(hFind, &file_data))
	{
		memcpy(pTempBuf + (i * iFileDataSize), &file_data, iFileDataSize);
		i++;
        memset(&file_data, 0, iFileDataSize);

	}


	FindClose(hFind);

end:
    (*pResult)->hdr.dwCommand = pMsg->hdr.dwCommand;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    if (E_SUCCESS == iError) {
        (*pResult)->hdr.dwMessageSize += dwAllocSize;
        (*pResult)->pData = pBuf;
    }

    return iError;


}

/*
@brief Helper function to count number of files in a given path
@param PCHAR diretory name
@param PINT pointer to an INT to return number of files
@return ERROR_T with status of dir
*/
INT CountFiles(PCHAR path, PINT iFileCount)
{
	WIN32_FIND_DATAA file_data = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;
	INT iError = E_SUCCESS;

	hFind = FindFirstFileA(path, &file_data);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		iError = E_DIR_ERROR;
		goto end;
	}

	(*iFileCount)++;

	while (FindNextFileA(hFind, &file_data))
	{
		(*iFileCount)++;
	}

end:
	FindClose(hFind);
	return iError;;
}