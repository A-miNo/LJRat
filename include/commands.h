#ifndef COMMANDS_H
#define COMMANDS_H

#include "error_code.h"
#include "message.h"
#include <windows.h>

ERROR_T GetCmd(PMESSAGE pMsg, PMESSAGE *pResult);
ERROR_T PutCmd(PMESSAGE pMsg, PMESSAGE *pResult);
ERROR_T ExecuteCmd(PMESSAGE pMsg, PMESSAGE *pResult);
ERROR_T DisconnectCmd(PMESSAGE pMsg, PMESSAGE *pResult);
ERROR_T DirCmd(PMESSAGE pMsg, PMESSAGE *pResult);
ERROR_T LoadCmd(PMESSAGE pMsg, PMESSAGE *pResult);

#endif // COMMANDS_H