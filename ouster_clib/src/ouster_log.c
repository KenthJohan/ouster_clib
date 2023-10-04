#include "ouster_clib/ouster_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

void ouster_log(char const *format, ...)
{
    assert(format);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}