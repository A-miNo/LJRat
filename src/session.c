#include <Winsock2.h>
#include <Windows.h>
#include "session.h"
#include "error.h"
#include "debug.h"

ERROR_T SessionInit(PSESSION_CTX pSession, INT af, INT type, INT protocol) {
    INT iError = E_SUCCESS;
    if (NULL == pSession) {
        goto end;
    }

    pSession->sock = WSASocketW(af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == pSession->sock)
    {
        DBG_PRINT("Error: %d\n", WSAGetLastError());
        goto end;
    }

end:
return iError;
}


ERROR_T SessionConnect(PSESSION_CTX pSession, SOCKADDR_IN *addr) {
    INT iError = E_SUCCESS;

    iError = WSAConnect(pSession->sock, (const SOCKADDR *)addr, sizeof(*addr), NULL, NULL, NULL, NULL);
    if (SOCKET_ERROR == iError) {
        DBG_PRINT("Error: %d\n", WSAGetLastError());
        goto end;
    }

end:
return iError;
}
