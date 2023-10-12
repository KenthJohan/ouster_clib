#pragma once
#include <stdint.h>
#include <stdlib.h>

int ouster_assert_(
	const char *expr, 
	const char *file, 
	int32_t line, 
	const char *fn, 
	const char *fmt, 
	...
	);




#define ouster_assert(expr, ...) ((expr) ? (void)0: (void)(ouster_assert_(#expr, __FILE__, __LINE__, __func__, __VA_ARGS__), abort()))
#define ouster_assert_notnull(expr) ouster_assert(expr, "%s", "Should not be NULL")