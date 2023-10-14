#include "ouster_clib/ouster_assert.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define COLOR_ASSERT "\033[38;2;255;50;50m" // Red
#define COLOR_FILE "\033[38;2;100;100;100m" // Grey
#define COLOR_LINE "\033[38;2;100;100;100m" // Grey
#define COLOR_EXPR "\033[38;2;220;150;220m" // Purple
#define COLOR_RST "\033[0m"



int ouster_assert_(
    const char *expr,
    const char *file,
    int32_t line,
    const char *fn,
    const char *fmt,
    ...)
{
	FILE * f = stderr;
	fprintf(f, COLOR_ASSERT"A "COLOR_RST);
	fprintf(f, COLOR_FILE"%s"COLOR_RST":"COLOR_LINE"%i"COLOR_RST" ", file, line);
	fprintf(f, COLOR_EXPR"(%s)"COLOR_RST" ", expr);
	va_list args;
	va_start(args, fmt);
	int r = vfprintf(f, fmt, args);
	va_end(args);
	fprintf(f,"\n");
	fflush(stdout);
	fflush(stderr);
	return r;
}