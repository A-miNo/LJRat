#ifndef WORKER_H
#define WORKER_H

#include <Windows.h>
#include "error_code.h"

/*
@brief Function used to process the commands received from the LP
@param Unused
@return ERROR_T with status of send
*/
unsigned int __stdcall WorkerThread(PVOID pArg);

#endif // WORKER_H