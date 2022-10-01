#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <error.h>
#include <winsock2.h>

#define PAYLOAD_HEADER_LEN 12

typedef struct _PAYLOAD_HEADER
{
    DWORD dwPayloadSize;
} PAYLOAD_HEADER, *PPAYLOAD_HEADER;

typedef struct _payload
{
    PAYLOAD_HEADER hdr;
    DWORD dwCommand;
    DWORD dwJobID;
    PVOID pData;
} PAYLOAD, *PPAYLOAD;

/*
Function that takes a buffer and a length and sends all of the data to the specified socket
*/
ERROR_T PayloadSend(PPAYLOAD pkt, SOCKET sock);

ERROR_T PayloadGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PPAYLOAD pPayload);

/*
Function to read a payload from a socket
*/
ERROR_T PayloadReceive(SOCKET sock, PPAYLOAD *payload);

#endif // PAYLOAD_H