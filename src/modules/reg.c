// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include "message.h"
#include "error_code.h"
#include "debug.h"

typedef enum _reg_t {
    REG_QUERY = 1
} REG_T;

typedef struct {
    DWORD dwNumKeys;
    DWORD dwNumValues;
} REG_HEADER, *PREG_HEADER;

ERROR_T RegQuery(PMESSAGE pMsg, PMESSAGE* pResult)
{
    INT iError = E_SUCCESS;

    DWORD dwName = 0;                   // size of name string 
    WCHAR achClass[MAX_PATH] = { 0 };  // buffer for class name 
    DWORD cchClassName = MAX_PATH;  // size of class string 
    DWORD cSubKeys = 0;               // number of subkeys 
    DWORD dwMaxSubKey = 0;              // longest subkey size 
    DWORD cchMaxClass = 0;              // longest class string 
    DWORD cValues = 0;              // number of values for key 
    DWORD dwMaxValueName = 0;          // longest value name 
    DWORD dwMaxValueData = 0;       // longest value data 
    DWORD dwSecurityDescriptor = 0; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
    DWORD i = 0, j = 0;
    DWORD cchValue;
    PDWORD pFuncCommand = (PDWORD)pMsg->pData;
    PDWORD pHive = (PBYTE) pMsg->pData + sizeof(DWORD);
    PDWORD pSubKeySize = (PBYTE)pMsg->pData + (sizeof(DWORD) * 2);
    PCHAR pSubKey = (PBYTE)pMsg->pData + (sizeof(DWORD) * 3);
    HANDLE hRegKey = INVALID_HANDLE_VALUE;

    // TODO: Possible future issue of allocating memory in a dll with static CRT that gets freed by another dll;
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/potential-errors-passing-crt-objects-across-dll-boundaries?view=msvc-170
    *pResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pResult) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    iError = RegOpenKeyExA(*pHive, pSubKey, 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hRegKey);
    if (iError != E_SUCCESS)
    {
        goto end;
    }

    // Get the class name and the value count. 
    iError = RegQueryInfoKeyA(
        hRegKey,                    // key handle 
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

    if (iError != E_SUCCESS)
    {
        goto end;
    }

    // Lets make a rough guess on how much memory we should allocate for a buffer to send results back

    DWORD dwAllocSize = 0;

    // Account for header size
    dwAllocSize = sizeof(REG_HEADER);

    // Account for all the subkeys
    dwAllocSize += cSubKeys * sizeof(DWORD); // Prefix size of subkey name
    dwAllocSize += cSubKeys * dwMaxSubKey;

    // Account for all the values
    dwAllocSize += cValues * sizeof(DWORD); // Prefix size of value name
    dwAllocSize += cValues * dwMaxValueName;
    dwAllocSize += cValues * sizeof(DWORD); // Prefix size of data
    dwAllocSize += cValues * dwMaxValueData;
    dwAllocSize += cValues * sizeof(DWORD); // value type

    PBYTE pBuf, pBufTemp = NULL;
    pBuf = (PBYTE) (*pResult)->pData;

    pBuf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAllocSize);
    if (NULL == pBuf)
	{
		iError = E_MEMORY_ERROR;
		goto end;
	}

    pBufTemp = pBuf;

    // Copy header to the buffer
    memcpy(pBufTemp, &cSubKeys, sizeof(DWORD));
    pBufTemp += sizeof(DWORD);
    memcpy(pBufTemp, &cValues, sizeof(DWORD));
    pBufTemp += sizeof(DWORD);


    // Lets allocate the max size of key and value to be safe
    // QueryInfoKey doesn't account for null so we are adding it to sizes
    dwMaxValueData += 1;
    dwMaxValueName += 1;
    dwMaxSubKey += 1;

    PBYTE pKeyName = NULL;

    pKeyName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwMaxSubKey);
    if (NULL == pKeyName)
    {
        goto end;
    }

    PBYTE pValueData = NULL;

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

    DWORD dwSubKeyNameSize = 0;
    if (cSubKeys)
    {

        for (i = 0; i < cSubKeys; i++)
        {
            dwSubKeyNameSize = dwMaxSubKey;
            iError = RegEnumKeyExA(
                hRegKey,
                i,
                (LPCSTR)pKeyName,
                &dwSubKeyNameSize,
                NULL,
                NULL,
                NULL,
                &ftLastWriteTime);

            if (iError != ERROR_SUCCESS && iError != ERROR_NO_MORE_ITEMS)
            {
                goto end;
            }
            #ifndef NDEBUG
            CHAR debugbuf[255];
            sprintf(debugbuf, "(%d) %s\n", i, (LPCSTR)pKeyName);
            OutputDebugStringA(debugbuf);
            #endif // Debug

            memcpy(pBufTemp, &dwSubKeyNameSize, sizeof(DWORD));
            pBufTemp += sizeof(DWORD);
            memcpy(pBufTemp, pKeyName, dwSubKeyNameSize);
            pBufTemp += dwSubKeyNameSize;

        }
    }

    // Enumerate the key values.

    DWORD dwType = 0;
    DWORD dwDataSize = 0;

    if (cValues)
    {
        //pMsg->pfprintf("\nNumber of values: %d\n", cValues);

        for (i = 0; i < cValues; i++)
        {
            dwDataSize = dwMaxValueData;
            cchValue = dwMaxValueName;
            iError = RegEnumValueA(
                hRegKey,
                i,
                (LPCSTR)pValueName,
                &cchValue,
                NULL,
                &dwType,
                pValueData,
                &dwDataSize);

            if (iError != ERROR_SUCCESS && iError != ERROR_NO_MORE_ITEMS)
            {
                #ifndef NDEBUG
                CHAR debugbuf[255];
                sprintf(debugbuf, "Failure: (%d)\n", iError);
                OutputDebugStringA(debugbuf);
                #endif // Debug
            goto end;
            }

            #ifndef NDEBUG
            CHAR debugbuf[16383];
            sprintf(debugbuf, "(%d) %s\n", i, (LPCSTR)pValueName);
            OutputDebugStringA(debugbuf);
            #endif // Debug

            memcpy(pBufTemp, &cchValue, sizeof(DWORD));
            pBufTemp += sizeof(DWORD);
            memcpy(pBufTemp, pValueName, cchValue);
            pBufTemp += cchValue;
            memcpy(pBufTemp, &dwType, sizeof(DWORD));
            pBufTemp += sizeof(DWORD);
            memcpy(pBufTemp, &dwDataSize, sizeof(DWORD));
            pBufTemp += sizeof(DWORD);
            memcpy(pBufTemp, pValueData, dwDataSize);
            pBufTemp += dwDataSize;


        }
    }

end:

    (*pResult)->hdr.dwCommand = pMsg->hdr.dwCommand;
    (*pResult)->hdr.dwJobID = pMsg->hdr.dwJobID;
    (*pResult)->hdr.dwMessageSize = sizeof(MESSAGE_HEADER);
    (*pResult)->hdr.dwResultCode = iError;

    if (E_SUCCESS == iError) {
        OutputDebugStringA("Success");
        (*pResult)->hdr.dwMessageSize += dwAllocSize;
        (*pResult)->pData = pBuf;
    }

    return iError;
}


#ifdef BUILD_EXE

int wmain(int argc, WCHAR* argv[])
{
    PMESSAGE pMsg, pResult;
    PFUNC_DATA pData;
    INT iError;
    CHAR regkey[] = "SOFTWARE\\Microsoft\\Windows\\currentversion";
    DWORD dwSize = sizeof(regkey);
    DWORD dwRegHive = 0x80000002;
    DWORD dwFuncCommand = 1;
    PBYTE pTemp = NULL;

    pMsg = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MESSAGE));
    if (NULL == pMsg) {
        iError = E_MEMORY_ERROR;
        goto end;
    }

    pMsg->pData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PATH);

    pTemp = pMsg->pData;

    memcpy(pTemp, &dwFuncCommand, sizeof(DWORD));
    pTemp += sizeof(DWORD);
    memcpy(pTemp, &dwRegHive, sizeof(DWORD));
    pTemp += sizeof(DWORD);
    memcpy(pTemp, &dwSize, sizeof(DWORD));
    pTemp += sizeof(DWORD);

    memcpy(pTemp, regkey, dwSize);


    switch (*(PDWORD) pMsg->pData) {

    case REG_QUERY:
        iError = RegQuery(pMsg, &pResult);
        break;

    default:
        break;
    }

end:
    return iError;
}

#else

__declspec(dllexport) ERROR_T func(PMESSAGE pMsg, PMESSAGE* pResult)
{
    INT iError = E_SUCCESS;

    switch (*(PDWORD)pMsg->pData) {

    case REG_QUERY:
        iError = RegQuery(pMsg, pResult);
        break;

    default:
        break;
    }

end:
    return iError;
}

#endif