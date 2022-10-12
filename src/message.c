#include "message.h"
#include "error.h"
#include "debug.h"
#include <windows.h>


ERROR_T PaylSend(PMESSAGE pMessage, SOCKET sock) {
    INT iError = E_SUCCESS;
    DWORD dwBytesSent = 0;
    WSABUF buf = {0};
    
    buf.buf = pMessage->pData;
    buf.len = pMessage->dwMessageSize;

    iError = WSASend(sock, &buf, 1, &dwBytesSent, 0, NULL, NULL);
    if (E_SUCCESS != iError) {
        iError = E_SEND_ERROR;
    }

    return iError;
}

ERROR_T MessageGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PMESSAGE pMessage) {
    INT iError = E_SUCCESS;

    if (NULL == pData || NULL == pMessage) {
        iError = E_NULL_ARG;
        goto end;
    }

    pMessage->dwMessageSize = MESSAGE_HEADER_LEN + dwDataLen;
    pMessage->pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMessage->dwMessageSize);
    
    PBYTE pCursor = pMessage->pData;
    
    *((PDWORD) pCursor) = dwDataLen;
    pCursor += sizeof(dwDataLen);
    *((PDWORD) pCursor) = dwCommandId;
    pCursor += sizeof(dwCommandId);
    *((PDWORD) pCursor) = dwCommand;
    pCursor += sizeof(dwCommand);

    memcpy(pCursor, pData, dwDataLen);

end:
    return iError;
}

ERROR_T MessageReceive(SOCKET sock, PMESSAGE *message) {
    PBYTE pData = NULL;
    PMESSAGE pMessage = NULL;
    WSABUF buf = {0};
    DWORD dwBytesReceived = 0;
    INT iError = E_SUCCESS;
    DWORD dwFlags = 0;

    pMessage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MESSAGE_HEADER_LEN);
    if (NULL == pMessage) 
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }
    
    buf.len = MESSAGE_HEADER_LEN;
    buf.buf = (PCHAR) pMessage;

    iError = WSARecv(sock, &buf, 1, &dwBytesReceived, &dwFlags, NULL, NULL);

    if (SOCKET_ERROR == iError)
    {
        goto end;
    }

    pMessage->dwMessageSize = pMessage->dwMessageSize;
    DBG_PRINT("Message size: %d\n", pMessage->dwMessageSize);

    pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMessage->dwMessageSize);
    if (NULL == pData)
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pMessage->pData = pData;
    buf.len = pMessage->dwMessageSize;
    buf.buf = pData;

    iError = WSARecv(sock, &buf, 1, &dwBytesReceived, &dwFlags, NULL, NULL);

    if (SOCKET_ERROR == iError)
    {
        goto end;
    }

    DBG_PRINT("Received %d bytes\n", dwBytesReceived);

end:

    if (SOCKET_ERROR == iError || E_MEMORY_ERROR == iError)
    {
        if (NULL != pMessage->pData)
        {
            HeapFree(GetProcessHeap(), 0, pMessage->pData);
            pMessage->pData = NULL;
        }

        if (NULL != pMessage)
        {
            HeapFree(GetProcessHeap(), 0, pMessage);
            pMessage = NULL;
        }

    }

    *message = pMessage;
    return iError;
}