/**
 * @defgroup log Logging
 * @brief Functionality logging
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_LOG_H
#define OUSTER_LOG_H

#include "ouster_clib.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Logging */
#ifdef OUSTER_ENABLE_LOG
void ouster_log_(char const *fmt, ...);
#define ouster_log(...) ouster_log_(__VA_ARGS__)
#else
#define ouster_log(...)
#endif

#ifdef __cplusplus
}
#endif

#endif // OUSTER_LOG_H

/** @} */