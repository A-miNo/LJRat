#include <Windows.h>
#include "Error.h"
#include "LJRat.h"

#ifndef BUILD_DLL

INT wmain(INT argc, WCHAR *argv[])
{
    Run();
}

#else

#define EXPORT_FUNC __declspec(dllexport)

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

EXPORT_FUNC ERROR_T Run(INT argc, CHAR *argv[])
{
    INT iError = E_SUCCESS;
    INT iStartup = E_SUCCESS;
    WSADATA wData;
    SOCKET sock;
    SOCKADDR_IN addr = {0};

    iStartup = WSAStartup(MAKEWORD(2,2), &wData);
    if (E_SUCCESS != iStartup)
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