#include "util.h"

//---------------------- Util--------------------------
// int panic(const char* format, ...) {
//     printf("\033[1;31m");
//     printf("panic: ");
//     va_list args;
//     va_start(args, format);
//     vprintf(format, args);
//     va_end(args);
//     printf("\033[0m");
//     exit(1);
// }

String makeString(const char* chars, int length) {
    String string;
    string.chars = malloc(length + 1);
    memcpy(string.chars, chars, length);
    string.chars[length] = '\0';
    string.length = length;
    return string;
}

bool stringEqual(String a, String b) {
    if (a.length != b.length) {
        return false;
    }
    return memcmp(a.chars, b.chars, a.length) == 0;
}

char* value(String string) { return string.chars; }