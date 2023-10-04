#include "ouster_clib/ouster_assert.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ouster_abort()
{
	abort();
}

int ouster_assert_(
    const char *expr,
    const char *file,
    int32_t line,
    const char *fn,
    const char *fmt,
    ...)
{
	va_list args;
	va_start(args, fmt);
	int r = vprintf(fmt, args);
	va_end(args);
	ouster_abort();
	return r;
}