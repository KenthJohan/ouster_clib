#include "ouster_clib/ouster_assert.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_ASSERT "\033[38;2;255;50;50m"
#define COLOR_RST "\033[0m"

void ouster_abort()
{
	printf("\n");
	fflush(stdout);
	fflush(stderr);
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
	printf(COLOR_ASSERT"ASSERT: "COLOR_RST"%s: ", expr);
	va_list args;
	va_start(args, fmt);
	int r = vprintf(fmt, args);
	va_end(args);
	ouster_abort();
	return r;
}