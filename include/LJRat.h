#ifndef LJRAT_H
#define LJRAT_H

#include "error_code.h"
#include "message.h"

#define SHUTDOWN_INITIATED 1
#define FUNCARRAYSIZE 32

typedef ERROR_T (*CMDFUNC)(PMESSAGE, PMESSAGE);


#endif // LJRAT_H