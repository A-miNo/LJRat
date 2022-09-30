#ifndef LJRAT_H
#define LJRAT_H

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Windows.h>
#include "Error.h"

#ifndef BUILD_DLL

    #define EXPORT_FUNC

#else

    #define EXPORT_FUNC __declspec(dllexport)

#endif // BUILD_DLL

#ifndef BUILD_DLL
EXPORT_FUNC ERROR_T Run(INT argc, CHAR *argv[]);
#else
EXPORT_FUNC ERROR_T Run(VOID);
#endif // BUILD_DLL

#endif // LJRAT_H