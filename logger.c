#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

void cchess_log(const char *msg, ...) {
    va_list varargs;
    va_start(varargs, msg);
    FILE *file = fopen(CCHESS_LOG, "a");
    vfprintf(file, msg, varargs);
    fclose(file);
}
