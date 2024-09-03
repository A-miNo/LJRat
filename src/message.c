#include "message.h"
#include "error_code.h"
#include "debug.h"
#include <windows.h>

static ERROR_T MessageSerialize(PMESSAGE pMessage, WSABUF *pBuf);

ERROR_T MessageSend(SOCKET sock, PMESSAGE pMessage) {
    INT iError = E_SUCCESS;
    DWORD dwBytesSent = 0;
    WSABUF buf = {0};
    
    iError = MessageSerialize(pMessage, &buf);
    if (E_SUCCESS != iError)
    {
        iError = E_SEND_ERROR;
        goto end;
    }

    iError = WSASend(sock, &buf, 1, &dwBytesSent, 0, NULL, NULL);
    if (E_SUCCESS != iError) {
        iError = E_SEND_ERROR;
    }

    DBG_PRINT("%d sent\n", dwBytesSent);

end:
    return iError;
}

ERROR_T MessageFree(PMESSAGE *pMsg) {
    INT iError = E_SUCCESS;

    if (NULL == *pMsg) {
        iError = E_NULL_ARG;
        goto end;
    }

    if (NULL != (*pMsg)->pData) {
        HeapFree(GetProcessHeap(), 0, (*pMsg)->pData);
    }

    HeapFree(GetProcessHeap(), 0, *pMsg);
    *pMsg = NULL;

end:
    return iError;

}

ERROR_T MessageSerialize(PMESSAGE pMessage, WSABUF *pBuf)
{
    INT iError = E_SUCCESS;

    if (NULL == pMessage->pData) {
        pBuf->buf = (PCHAR) &pMessage->hdr;
        pBuf->len = sizeof(MESSAGE_HEADER);
    }
    else {
        pBuf->buf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMessage->hdr.dwMessageSize);
        if (NULL == pBuf->buf) {
            iError = E_MEMORY_ERROR;
            goto end;
        }
        else {
            memcpy(pBuf->buf, pMessage, sizeof(MESSAGE_HEADER));
            memcpy(pBuf->buf + sizeof(MESSAGE_HEADER), pMessage->pData, pMessage->hdr.dwMessageSize - sizeof(MESSAGE_HEADER));
            HeapFree(GetProcessHeap(), 0, pMessage->pData);
            pMessage->pData = NULL;
        }
        pBuf->len = pMessage->hdr.dwMessageSize;
    }

end:
    return iError;
}

ERROR_T MessageGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PMESSAGE pMessage) {
    INT iError = E_SUCCESS;

    if (NULL == pData || NULL == pMessage) {
        iError = E_NULL_ARG;
        goto end;
    }

    pMessage->hdr.dwMessageSize = sizeof(MESSAGE_HEADER) + dwDataLen;
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
    DWORD dwMessageSize = 0;
    DWORD dwFlags = 0;
    DWORD dwBytesRecvCounter = 0;

    pMessage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pMessage) 
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }
    
    buf.len = sizeof(MESSAGE_HEADER);
    buf.buf = (PCHAR) pMessage;

    iError = WSARecv(sock, &buf, 1, &dwBytesReceived, &dwFlags, NULL, NULL);

    if (SOCKET_ERROR == iError)
    {
        goto end;
    }

    dwMessageSize = pMessage->hdr.dwMessageSize - sizeof(MESSAGE_HEADER);
    DBG_PRINT("Message size: %d\n", dwMessageSize);

    pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwMessageSize);
    if (NULL == pData)
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pMessage->pData = pData;
    buf.len = dwMessageSize;
    buf.buf = pData;

    while (dwMessageSize != dwBytesRecvCounter)
    {
        iError = WSARecv(sock, &buf, 1, &dwBytesReceived, &dwFlags, NULL, NULL);
        if (SOCKET_ERROR == iError)
        {
            DBG_PRINT("Socket RECV ret: %d\n", iError);
            goto end;
        }

        dwBytesRecvCounter += dwBytesReceived;

        if (dwMessageSize != dwBytesRecvCounter)
        {
            buf.buf += dwBytesReceived;
            buf.len -= dwBytesReceived;
            dwBytesReceived = 0;
        }

    }

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