#ifndef MESSAGE_H
#define MESSAGE_H

#include <error.h>
#include <winsock2.h>

#define MESSAGE_HEADER_LEN 16

typedef struct _message_header {
    DWORD dwMessageSize;
    DWORD dwJobID;
    DWORD dwCommand;
    DWORD dwResultCode;
} MESSAGE_HEADER, *PMESSAGE_HEADER;

typedef struct _message {
    MESSAGE_HEADER hdr;
    PVOID pData;
} MESSAGE, *PMESSAGE;

/*
Function that takes a buffer and a length and sends all of the data to the specified socket
*/
ERROR_T MessageSend(SOCKET sock, PMESSAGE pMessage);

ERROR_T MessageGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PMESSAGE pMessage);

/*
Function to read a payload from a socket
*/
ERROR_T MessageReceive(SOCKET sock, PMESSAGE *message);

#endif // MESSAGE_H