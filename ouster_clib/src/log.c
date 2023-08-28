#include "ouster_clib/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

void ouster_log(char *format, ...)
{
    assert(format);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}