/**
 * @defgroup os_api OS API
 * @brief API of OS
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_OUSTER_OS_API_H
#define OUSTER_OUSTER_OS_API_H

#include "ouster_clib.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*ouster_os_api_malloc_t)(size_t size);

typedef void* (*ouster_os_api_realloc_t)(void *ptr, size_t size);

typedef void* (*ouster_os_api_calloc_t)(size_t size);

typedef void (*ouster_os_api_free_t)(void *ptr);

typedef void (*ouster_os_api_log_t)(int32_t level, const char *file, int32_t line, const char *msg);

typedef void (*ouster_os_api_abort_t)(void);

typedef struct ouster_os_api_t {

	/* Memory management */
	ouster_os_api_malloc_t malloc_;
	ouster_os_api_realloc_t realloc_;
	ouster_os_api_calloc_t calloc_;
	ouster_os_api_free_t free_;

	/* Logging */
	ouster_os_api_log_t log_;

	/* Application termination */
	ouster_os_api_abort_t abort_;
} ouster_os_api_t;

extern ouster_os_api_t ouster_os_api;


#ifndef ouster_os_malloc
#define ouster_os_malloc(size) ouster_os_api.malloc_(size)
#endif

#ifndef ouster_os_free
#define ouster_os_free(ptr) ouster_os_api.free_(ptr)
#endif

#ifndef ouster_os_realloc
#define ouster_os_realloc(ptr, size) ouster_os_api.realloc_(ptr, size)
#endif

#ifndef ouster_os_calloc
#define ouster_os_calloc(size) ouster_os_api.calloc_(size)
#endif

#ifndef ouster_os_abort
#define ouster_os_abort(size) ouster_os_api.abort_()
#endif


/* Logging */
void ouster_os_dbg(const char *file, int32_t line, const char *msg);
void ouster_os_trace(const char *file, int32_t line, const char *msg);
void ouster_os_warn(const char *file, int32_t line, const char *msg);
void ouster_os_err(const char *file, int32_t line, const char *msg);
void ouster_os_fatal(const char *file, int32_t line, const char *msg);

void ouster_os_set_api_defaults(void);



#ifdef __cplusplus
}
#endif

#endif // OUSTER_OUSTER_OS_API_H

/** @} */