#include "ouster_clib/ouster_os_api.h"

ouster_os_api_t ouster_os_api;

void ouster_os_dbg(const char *file, int32_t line, const char *msg)
{
	if (ouster_os_api.log_) {
		ouster_os_api.log_(1, file, line, msg);
	}
}

void ouster_os_trace(const char *file, int32_t line, const char *msg)
{
	if (ouster_os_api.log_) {
		ouster_os_api.log_(0, file, line, msg);
	}
}

void ouster_os_warn(const char *file, int32_t line, const char *msg)
{
	if (ouster_os_api.log_) {
		ouster_os_api.log_(-2, file, line, msg);
	}
}

void ouster_os_err(const char *file, int32_t line, const char *msg)
{
	if (ouster_os_api.log_) {
		ouster_os_api.log_(-3, file, line, msg);
	}
}

void ouster_os_fatal(const char *file, int32_t line, const char *msg)
{
	if (ouster_os_api.log_) {
		ouster_os_api.log_(-4, file, line, msg);
	}
}

static void *ouster_os_api_calloc(size_t size)
{
	return calloc(1, (size_t)size);
}

static void *ouster_os_api_malloc(size_t size)
{
	return malloc((size_t)size);
}

static void ouster_os_api_free(void *ptr)
{
	free(ptr);
}

static void * ouster_os_api_realloc(void *ptr, size_t size)
{
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
	fputs("\n", stream);
}

void ouster_os_set_api_defaults(void)
{
	/* Memory management */
	ouster_os_api.malloc_ = ouster_os_api_malloc;
	ouster_os_api.free_ = ouster_os_api_free;
	ouster_os_api.realloc_ = ouster_os_api_realloc;
	ouster_os_api.calloc_ = ouster_os_api_calloc;

	/* Logging */
	ouster_os_api.log_ = ouster_log_msg;

	ouster_os_api.abort_ = abort;
}