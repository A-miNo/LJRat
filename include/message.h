#ifndef MESSAGE_H
#define MESSAGE_H

#define WIN32_LEAN_AND_MEAN

#include <error_code.h>
#include <winsock2.h>
#include <stdio.h>

#define MESSAGE_HEADER_LEN 16
typedef PVOID(*PHEAPALLOCFUNC)(HANDLE, DWORD, size_t);

typedef struct _message_header {
    DWORD dwMessageSize;
    DWORD dwJobID;
    DWORD dwCommand;
    DWORD dwResultCode;
} MESSAGE_HEADER, *PMESSAGE_HEADER;

typedef struct _message {
    MESSAGE_HEADER hdr;
    PVOID pData;
    PHEAPALLOCFUNC pHeapAlloc;
} MESSAGE, *PMESSAGE;

/*
@brief Function that takes a buffer and a length and sends all of the data to the specified socket
@param SOCKET of connection
@param PMESSAGE with data to send
@return ERROR_T with status of send
*/
ERROR_T MessageSend(SOCKET sock, PMESSAGE pMessage);

/*
@brief Function that allocates memory and assigns paramaters to the message
@param DWORD of unique command ID
@param DWORD of command type
@param PVOID of data to be sent back
@param DWORD size of pData
@param PMESSAGE of allocated message to be created
@return ERROR_T with status of send
*/
ERROR_T MessageGenerate(DWORD dwCommandId, DWORD dwCommand, PVOID pData, DWORD dwDataLen, PMESSAGE pMessage);

/*
@brief Function to read a payload from a socket
@param SOCKET of connection
@param PMESSAGE to store data read
@return ERROR_T with status of send
*/
ERROR_T MessageReceive(SOCKET sock, PMESSAGE *message);

#endif // MESSAGE_H