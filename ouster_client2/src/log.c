#include "log.h"
#include <stdarg.h>
#include <stdio.h>

void ouster_log(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}