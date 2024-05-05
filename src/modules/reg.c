#include <Windows.h>
#include "error.h"
#include "common.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

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
    PCOMMON pCommon = (PCOMMON) pArg;
    PREG_DATA pReg = (PREG_DATA) pCommon->pData;

    switch (pReg->dwRegCommand) {

        case REG_QUERY:
            iError = RegQuery(pReg);
            break;

        default:
            break;
    }

end:
    return iError;
}

static  ERROR_T RegQuery(PREG_DATA pReg)
{
    HANDLE hKey = INVALID_HANDLE_VALUE;
    INT iError = E_SUCCESS;
    
    iError = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft", 0, KEY_READ, &hKey);
    if (E_SUCCESS != iError)
    {
        goto end;
    }

    WCHAR    wKey[MAX_KEY_LENGTH] = {0};   // buffer for subkey name
    DWORD    dwName = 0;                   // size of name string 
    WCHAR    wClass[MAX_PATH] = {0};  // buffer for class name 
    DWORD    dwClassName = MAX_PATH;  // size of class string 
    DWORD    dwSubKeyCount = 0;               // number of subkeys 
    DWORD    dwMaxSubKeyLen = 0;              // longest subkey size 
    DWORD    dwMaxClassLen = 0;              // longest class string 
    DWORD    dwValues = 0;              // number of values for key 
    DWORD    dwMaxValueNameLen = 0;          // longest value name 
    DWORD    dwMaxValueData = 0;       // longest value data 
    DWORD    dwSecurityDescriptor = 0; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    RegQueryInfoKeyW(
        hKey,
        wKey,
        &dwName,
        NULL,
        wClass,
        &dwSubKeyCount,
        &dwMaxSubKeyLen,
        NULL,
        &dwValues,
        &dwMaxValueNameLen,
        &dwMaxValueData,
        &dwSecurityDescriptor,
        &ftLastWriteTime        
        );

        
end:
    return iError;
}