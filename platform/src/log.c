#include "platform/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

void platform_log(char const *format, ...)
{
    assert(format);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}