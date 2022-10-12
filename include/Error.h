#ifndef ERROR_H
#define ERROR_H

typedef enum _error_t {
    E_SUCCESS = 0,
    E_FAILURE,
    E_MEMORY_ERROR,
    E_SEND_ERROR,
    E_NULL_ARG,
    E_JOB_REMOVE_ERROR,
    E_THREAD_ERROR,
    E_WINSOCK_ERROR,
    E_GET_READ_ERROR,
    E_GET_SIZE_ERROR,
    E_GET_SIZE_LARGE

} ERROR_T;

#endif // ERROR_H