#ifndef ERROR_H
#define ERROR_H

// TOOD implement logic that separates critical errors from informational
typedef enum _error_t {
    E_SUCCESS = 0x0000,
    E_FAILURE = 0x0001,
    E_FILE_EXISTS_ERROR = 0x1000,
    E_GET_ERROR = 0x1001,
    E_PUT_ERROR = 0x1002,
    E_DIR_ERROR = 0x1003

} ERROR_T;

typedef enum _internal_error_t {
    E_MEMORY_ERROR = 0x2000,
    E_SEND_ERROR,
    E_NULL_ARG,
    E_JOB_REMOVE_ERROR,
    E_THREAD_ERROR,
    E_WINSOCK_ERROR,
} INTERNAL_ERROR_T;
#endif // ERROR_H