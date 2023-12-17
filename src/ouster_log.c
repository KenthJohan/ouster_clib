#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_os_api.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

static char *ouster_vasprintf_malloc(const char *fmt, va_list args)
{
	size_t size = 0;
	char *result = NULL;
	va_list tmpa;

	va_copy(tmpa, args);

	size = vsnprintf(result, 0, fmt, tmpa);

	va_end(tmpa);

	if ((int32_t)size < 0) {
		return NULL;
	}

	result = (char *)ouster_os_malloc(size + 1);

	if (!result) {
		return NULL;
	}

	vsprintf(result, fmt, args);

	return result;
}

void ouster_log_(int32_t level, char const *file, int32_t line, char const *fmt, ...)
{
	assert(fmt);
	va_list args;
	va_start(args, fmt);

	char *msg = ouster_vasprintf_malloc(fmt, args);
	ouster_os_api.log_(0, file, line, msg);
	ouster_os_free(msg);

	va_end(args);
}