#include "ouster_clib/ouster_os_api.h"
#include <stdio.h>
#include <stdlib.h>

ouster_os_api_t ouster_os_api;
int64_t ouster_os_api_malloc_count = 0;
int64_t ouster_os_api_realloc_count = 0;
int64_t ouster_os_api_calloc_count = 0;
int64_t ouster_os_api_free_count = 0;

static void *ouster_os_api_calloc(size_t size)
{
	ouster_os_api_calloc_count++;
	return calloc(1, (size_t)size);
}

static void *ouster_os_api_malloc(size_t size)
{
	ouster_os_api_malloc_count++;
	return malloc((size_t)size);
}

static void ouster_os_api_free(void *ptr)
{
	ouster_os_api_free_count++;
	free(ptr);
}

static void * ouster_os_api_realloc(void *ptr, size_t size)
{
	ouster_os_api_realloc_count++;
	return realloc(ptr, size);
}

static void ouster_log_msg(int32_t level, const char *file, int32_t line, const char *msg)
{
	FILE *stream;
	if (level >= 0) {
		stream = stdout;
	} else {
		stream = stderr;
	}
	fputs(msg, stream);
	//fputs("\n", stream);
}

void ouster_os_set_api_defaults(void)
{
	/* Memory management */
	ouster_os_api.malloc_ = ouster_os_api_malloc;
	ouster_os_api.free_ = ouster_os_api_free;
	ouster_os_api.realloc_ = ouster_os_api_realloc;
	ouster_os_api.calloc_ = ouster_os_api_calloc;

	/* Logging */
	ouster_os_api.log_level_ = -1;
	ouster_os_api.log_ = ouster_log_msg;

	ouster_os_api.abort_ = abort;
}