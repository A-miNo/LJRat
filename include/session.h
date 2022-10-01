#ifndef SESSION_H
#define SESSION_H

#include <Windows.h>
#include "error.h"

typedef struct _session {
    SOCKET sock;
} SESSION, *PSESSION;

// Function to initialize the type of session, currently only TCP is supported
// @param pSession is a pointer to the session to be initialized
ERROR_T SessionInit(PSESSION pSession, INT af, INT type, INT protocol);

// Function to connect to the endpoint of a session
// @param pSession is a pointer to the session to use to connect
ERROR_T SessionConnect(PSESSION pSession, SOCKADDR_IN *addr);

#endif // SESSION_H