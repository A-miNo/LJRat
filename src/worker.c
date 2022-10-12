#include "LJRat.h"
#include "worker.h"
#include "list.h"
#include "error.h"
#include "message.h"
#include "debug.h"
#include "session.h"

extern SESSION_CTX session_ctx;

enum CMD_TYPE {
    GET,
    PUT,
    EXECUTE
};

static ERROR_T ProcessWork(PMESSAGE pMsg);
static ERROR_T GetCmd(PMESSAGE pMsg);
static ERROR_T PutCmd(PMESSAGE pMsg);
static ERROR_T ExecuteCmd(PMESSAGE pMsg);

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
    } 

end:
    return iError;
}

ERROR_T ProcessWork(PMESSAGE pMsg)
{
    INT iError = E_SUCCESS;

    DBG_PRINT("Len: %d CMD_TYPE: %d JOB_ID: %d\n", pMsg->dwMessageSize, pMsg->dwCommand, pMsg->dwJobID);

    switch(pMsg->dwCommand) {
        case GET:
            iError = GetCmd(pMsg);
            break;
        case PUT:
            iError = PutCmd(pMsg);
            break;
        case EXECUTE:
            iError = ExecuteCmd(pMsg);
            break;

        default:
            break;
    }

    return iError;
}

ERROR_T GetCmd(PMESSAGE pMsg) {
    INT iError = E_SUCCESS;
    PMESSAGE pResult = NULL;

    pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }



end:
    return iError;
}

ERROR_T PutCmd(PMESSAGE pMsg) {
    INT iError = E_SUCCESS;
    PMESSAGE pResult = NULL;

    pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

end:
    return iError;
}

ERROR_T ExecuteCmd(PMESSAGE pMsg) {
    INT iError = E_SUCCESS;
    PMESSAGE pResult = NULL;

    pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

end:
    return iError;
}