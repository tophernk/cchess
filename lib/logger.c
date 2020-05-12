#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

void __vcchess_log_file(const char *, va_list);

void cchess_log(const char *msg, ...) {
    va_list varargs;
    va_start(varargs, msg);

    switch (CCHESS_LOG_LEVEL) {
        case LOG_FILE: {
            __vcchess_log_file(msg, varargs);
        }
        case LOG_STDOUT: {
            vprintf(msg, varargs);
        }
        default: {
            break;
        }
    }
}


void cchess_log_file(const char *msg, ...) {
    va_list varargs;
    va_start(varargs, msg);
    __vcchess_log_file(msg, varargs);
}

void __vcchess_log_file(const char *msg, va_list args) {
    FILE *file = fopen(CCHESS_LOG, "a");
    vfprintf(file, msg, args);
    fclose(file);
}
