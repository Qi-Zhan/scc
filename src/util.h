#ifndef UTIL_H
#define UTIL_H

#define bool _Bool
#define true 1
#define false 0

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define panic(format, ...)                      \
    do {                                        \
        printf("[%s:%d] ", __FILE__, __LINE__); \
        printf("\033[1;31m");                   \
        printf("panic: ");                      \
        printf(format, ##__VA_ARGS__);          \
        printf("\033[0m");                      \
        exit(1);                                \
    } while (0)  // do { ... } while (0) is a common C idiom for macros that
                 // contain multiple statements and must be used with a
                 // semicolon

// define string type
typedef struct {
    char* chars;
    int length;
} String;

String makeString(const char* chars, int length);

bool stringEqual(String a, String b);

char* value(String string);

#define assert(condition)                            \
    if (!(condition)) {                              \
        panic("assertion failed: %s\n", #condition); \
    }

#define assertMsg(condition, message)                \
    if (!(condition)) {                              \
        panic("assertion failed: %s\n", #condition); \
        panic("message: %s\n", message);             \
    }

#endif
