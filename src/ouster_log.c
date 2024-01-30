#include "ouster_clib.h"
#include "ouster_math.h"

#include <stdarg.h>
#include <stdio.h>

int32_t ouster_log_set_level(int32_t level)
{
    int prev = level;
    ouster_os_api.log_level_ = level;
    return prev;
}

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
	ouster_assert_notnull(fmt);

    if (level > ouster_os_api.log_level_) {
        return;
    }

	va_list args;
	va_start(args, fmt);

	char *msg = ouster_vasprintf_malloc(fmt, args);
	ouster_assert_notnull(msg);
	ouster_os_api.log_(0, file, line, msg);
	ouster_os_free(msg);

	va_end(args);
}


void ouster_log_m4_(double const a[16])
{
	ouster_log(OUSTER_M4_FORMAT, OUSTER_M4_ARGS_1(a));
}

void ouster_log_m3_(double const a[9])
{
	ouster_log(OUSTER_M3_FORMAT, OUSTER_M3_ARGS_1(a));
}

void ouster_log_v3_(double const a[3])
{
	ouster_log(OUSTER_V3_FORMAT, OUSTER_V3_ARGS(a));
}