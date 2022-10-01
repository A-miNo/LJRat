#include "payload.h"
#include "error.h"
#include "debug.h"
#include <windows.h>


ERROR_T PayloadSend(PPAYLOAD pPayload, SOCKET sock) {
    INT iError = E_SUCCESS;
    DWORD dwBytesSent = 0;
    WSABUF buf = {0};
    
    buf.buf = pPayload->pData;
    buf.len = pPayload->hdr.dwPayloadSize;

    iError = WSASend(sock, &buf, 1, &dwBytesSent, 0, NULL, NULL);
    if (E_SUCCESS != iError) {
        iError = E_SEND_ERROR;
    }

    return iError;
}

ERROR_T PayloadGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PPAYLOAD pPayload) {
    INT iError = E_SUCCESS;

    if (NULL == pData || NULL == pPayload) {
        iError = E_NULL_ARG;
        goto end;
    }

    pPayload->hdr.dwPayloadSize = PAYLOAD_HEADER_LEN + dwDataLen;
    pPayload->pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pPayload->hdr.dwPayloadSize);
    
    PBYTE pCursor = pPayload->pData;
    
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

ERROR_T PayloadReceive(SOCKET sock, PPAYLOAD *payload) {
    PBYTE pData = NULL;
    PPAYLOAD pPayload = NULL;
    WSABUF buf = {0};
    DWORD dwBytesReceived = 0;
    INT iError = E_FAILURE;
    DWORD dwFlags = 0;

    pPayload = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PAYLOAD));
    if (NULL == pPayload) 
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }
    
    buf.len = sizeof(PAYLOAD_HEADER);
    buf.buf = (PCHAR) &pPayload->hdr;

    iError = WSARecv(sock, &buf, 1, &dwBytesReceived, &dwFlags, NULL, NULL);

    if (SOCKET_ERROR == iError)
    {
        goto end;
    }

    pPayload->hdr.dwPayloadSize = htonl(pPayload->hdr.dwPayloadSize);
    DBG_PRINT("Payload size: %d\n", pPayload->hdr.dwPayloadSize);

    pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pPayload->hdr.dwPayloadSize);
    if (NULL == pData)
    {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pPayload->pData = pData;
    buf.len = pPayload->hdr.dwPayloadSize;
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
        if (NULL != pPayload->pData)
        {
            HeapFree(GetProcessHeap(), 0, pPayload->pData);
            pPayload->pData = NULL;
        }

        if (NULL != pPayload)
        {
            HeapFree(GetProcessHeap(), 0, pPayload);
            pPayload = NULL;
        }

    }

    *payload = pPayload;
    return iError;
}