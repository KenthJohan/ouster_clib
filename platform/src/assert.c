#include "platform/assert.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void platform_abort()
{
	abort();
}

int platform_assert_(
	const char *expr, 
	const char *file, 
	int32_t line, 
	const char *fn, 
	const char *fmt, 
	...
	)
{
	va_list args;
	va_start(args, fmt);
	int r = vprintf(fmt, args);
	va_end(args);
	platform_abort();
	return r;
}