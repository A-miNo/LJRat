#ifndef MESSAGE_H
#define MESSAGE_H

#include <error.h>
#include <winsock2.h>

#define MESSAGE_HEADER_LEN 12

typedef struct _MESSAGE_HEADER
{
    DWORD dwMessageSize;
} MESSAGE_HEADER, *PMESSAGE_HEADER;

typedef struct _payload
{
    MESSAGE_HEADER hdr;
    DWORD dwCommand;
    DWORD dwJobID;
    PVOID pData;
} MESSAGE, *PMESSAGE;

/*
Function that takes a buffer and a length and sends all of the data to the specified socket
*/
ERROR_T MessageSend(PMESSAGE pkt, SOCKET sock);

ERROR_T MessageGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PMESSAGE pMessage);

/*
Function to read a payload from a socket
*/
ERROR_T MessageReceive(SOCKET sock, PMESSAGE *message);

#endif // MESSAGE_H