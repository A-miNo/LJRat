#include <winsock2.h>
#include <Windows.h>
#include "error.h"
#include "LJRat.h"

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
    SOCKADDR_IN addr = {0};

    iStartup = WSAStartup(MAKEWORD(2,2), &wData);
    if (E_SUCCESS != iStartup)
    {
        goto cleanup;
    }

addr.sin_family = AF_INET;

#ifndef BUILD_DLL
dwPort = atoi(argv[2]);

if (dwPort < 1 || dwPort > 65535)
{
    goto cleanup;
}

addr.sin_port = (WORD)dwPort;
addr.sin_addr.s_addr = inet_addr(argv[1]);
#else
addr.sin_port = htons(TARGET_PORT);
addr.sin_addr.s_addr = inet_addr(TARGET_IP);
#endif // BUILD_DLL

if (INADDR_NONE == addr.sin_addr.s_addr)
{
    goto cleanup;
}

cleanup:
if (0 == iStartup)
{
    WSACleanup();
}

return iError;
}