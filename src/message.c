#include "message.h"
#include "error.h"
#include "debug.h"
#include <windows.h>


ERROR_T PaylSend(PMESSAGE pMessage, SOCKET sock) {
    INT iError = E_SUCCESS;
    DWORD dwBytesSent = 0;
    WSABUF buf = {0};
    
    buf.buf = pMessage->pData;
    buf.len = pMessage->hdr.dwMessageSize;

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

    pMessage->hdr.dwMessageSize = MESSAGE_HEADER_LEN + dwDataLen;
    pMessage->pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMessage->hdr.dwMessageSize);
    
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

    pMessage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pMessage) 
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }
    
    buf.len = sizeof(MESSAGE_HEADER);
    buf.buf = (PCHAR) &pMessage->hdr;

    iError = WSARecv(sock, &buf, 1, &dwBytesReceived, &dwFlags, NULL, NULL);

    if (SOCKET_ERROR == iError)
    {
        goto end;
    }

    pMessage->hdr.dwMessageSize = htonl(pMessage->hdr.dwMessageSize);
    DBG_PRINT("Message size: %d\n", pMessage->hdr.dwMessageSize);

    pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMessage->hdr.dwMessageSize);
    if (NULL == pData)
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pMessage->pData = pData;
    buf.len = pMessage->hdr.dwMessageSize;
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