/**
 * @defgroup assert Assertion
 * @brief Functionality for sanity checks
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_ASSERT_H
#define OUSTER_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OUSTER_DEBUG

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

/** Assert. 
 * Aborts if condition is false, disabled in debug mode. */
#define ouster_assert(expr, ...) ((expr) ? (void)0: (void)(ouster_assert_(#expr, __FILE__, __LINE__, __func__, __VA_ARGS__), abort()))

/** Assert. 
 * Aborts if condition is false, disabled in debug mode. */
#define ouster_assert_notnull(expr) ouster_assert(expr, "%s", "Should not be NULL")


#else
#define ouster_assert(expr, ...)
#define ouster_assert_notnull(expr)
#endif // OUSTER_DEBUG

#ifdef __cplusplus
}
#endif

#endif // OUSTER_ASSERT_H

/** @} */