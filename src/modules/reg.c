#include <Windows.h>
#include "error.h"

typedef struct reg_data
{
    DWORD dwRegCommand;
} REG_DATA, *PREG_DATA;

typedef enum _reg_t {
    REG_QUERY = 1
} REG_T;

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

__declspec(dllexport) ERROR_T dll_func(PVOID pArg)
{
    INT iError = E_SUCCESS;

    PREG_DATA pReg = (PREG_DATA) pArg;
    switch (pReg->dwRegCommand) {

        case REG_QUERY:
           iError = RegQuery(pArg);
            break;

        default:
            break;
    }

end:
    return iError;
}

ERROR_T RegQuery(PVOID pArg)
{
    INT iError = ERROR_SUCCESS;

    if (NULL == pArg)
    {
        iError = E_FAILURE;
        goto end;
    }

end:
    return iError;

}