#pragma once
#include <stdint.h>


int platform_assert_(
	const char *expr, 
	const char *file, 
	int32_t line, 
	const char *fn, 
	const char *fmt, 
	...
	);


#define platform_assert(expr, ...) ((expr) ? 0: platform_assert_ (#expr, __FILE__, __LINE__, __func__, __VA_ARGS__))
#define platform_assert_notnull(expr) platform_assert(expr, "")