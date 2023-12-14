#include "ouster_clib/ouster_log.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void ouster_log_(char const *format, ...)
{
	assert(format);
	printf("OUSTER: ");
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}