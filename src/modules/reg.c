// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include "common.h"
#include "message.h"
#include "error_code.h"
#include "debug.h"

#define W_NULL_SIZE 2

typedef struct reg_data
{
    DWORD dwRegCommand;
} REG_DATA, *PREG_DATA;

typedef enum _reg_t {
    REG_QUERY = 1
} REG_T;

ERROR_T RegQuery(PREG_DATA pRegData)
{
    INT iError = E_SUCCESS;

    HANDLE hKey = INVALID_HANDLE_VALUE;
    DWORD dwName = 0;                   // size of name string 
    WCHAR achClass[MAX_PATH] = {0};  // buffer for class name 
    DWORD cchClassName = MAX_PATH;  // size of class string 
    DWORD cSubKeys = 0;               // number of subkeys 
    DWORD dwMaxSubKey = 0;              // longest subkey size 
    DWORD cchMaxClass = 0;              // longest class string 
    DWORD cValues = 0;              // number of values for key 
    DWORD dwMaxValueName = 0;          // longest value name 
    DWORD dwMaxValueData = 0;       // longest value data 
    DWORD dwSecurityDescriptor = 0; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i = 0, j = 0, retCode = 0;

    DWORD cchValue;

    // Get the class name and the value count. 
    retCode = RegQueryInfoKeyW(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &dwMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &dwMaxValueName,            // longest value name 
        &dwMaxValueData,         // longest value data 
        &dwSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 


    // Lets allocate the max size of key and value to be safe
    
    PBYTE pKeyName = NULL;
    dwMaxSubKey += W_NULL_SIZE;
    pKeyName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwMaxSubKey);
    if (NULL == pKeyName)
    {
        goto end;
    }

    PBYTE pValueData = NULL;
    dwMaxValueData += W_NULL_SIZE;
    pValueData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwMaxValueData);
    if (NULL == pKeyName)
    {
        goto end;
    }

    PBYTE pValueName = NULL;
    pValueName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwMaxValueName);
    if (NULL == pKeyName)
    {
        goto end;
    }


    // Enumerate the subkeys, until RegEnumKeyEx fails.

    if (cSubKeys)
    {

        for (i = 0; i < cSubKeys; i++)
        {
            retCode = RegEnumKeyExW(
                hKey,
                i,
                (LPWSTR) pKeyName,
                &dwMaxSubKey,
                NULL,
                NULL,
                NULL,
                &ftLastWriteTime);

            if (retCode == ERROR_SUCCESS)
            {
                DBG_PRINT("(%d) %ls\n", i + 1, (LPWSTR) pKeyName);
            }
        }
    }

    // Enumerate the key values.

    DWORD dwType = 0;
    DWORD dwDataSize = 0;

    if (cValues)
    {
        DBG_PRINT("\nNumber of values: %d\n", cValues);

        for (i = 0; i < cValues; i++)
        {
            dwDataSize = dwMaxValueData;
            cchValue = dwMaxValueName;
            retCode = RegEnumValueW(
                hKey,
                i,
                (LPWSTR) pValueName,
                &cchValue,
                NULL,
                &dwType,
                pValueData,
                &dwDataSize);

            if (retCode == ERROR_SUCCESS)
            {
                DBG_PRINT("Name: %ls Type: %d\n", (LPWSTR) pValueName, dwType);
            }
        }
    }

end:
    return iError;
}

#ifdef BUILD_EXE

int wmain(int argc, WCHAR *argv[])
{
    HKEY hTestKey = 0;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
        argv[1],
        0,
        KEY_READ,
        &hTestKey) == ERROR_SUCCESS
        )
    {
        QueryKey(hTestKey);
    }

    RegCloseKey(hTestKey);

    return 0;
}

#else

__declspec(dllexport) ERROR_T dll_func(PMESSAGE pMsg, PMESSAGE *pResult)
{
    INT iError = E_SUCCESS;
    PREG_DATA pReg = (PREG_DATA) pMsg->pData;

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

#endif