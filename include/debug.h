#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#define DEBUG 1

#include <stdio.h>

#define DBG_PRINT(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)
#else DEBUG
#define DBG_PRINT(...) do{ } while (0)
#endif DEBUG

#endif DEBUG_H