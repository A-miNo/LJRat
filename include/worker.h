#ifndef WORKER_H
#define WORKER_H

#include <Windows.h>
#include "error.h"

// Function used to process the commands received from the LP
unsigned int __stdcall WorkerThread(PVOID pArg);

#endif // WORKER_H