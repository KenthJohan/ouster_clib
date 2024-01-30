/**
 * @defgroup log Logging
 * @brief Functionality logging
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_LOG_H
#define OUSTER_LOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


int32_t ouster_log_set_level(int32_t level);

/** Logging */
#ifdef OUSTER_ENABLE_LOG
void ouster_log_(int32_t level, char const * file, int32_t line, char const *fmt, ...);
void ouster_log_m4_(double const a[16]);
void ouster_log_m3_(double const a[9]);
void ouster_log_v3_(double const a[3]);
#define ouster_log(...) ouster_log_(0, __FILE__, __LINE__, __VA_ARGS__)
#define ouster_log_m4(...) ouster_log_m4_(__VA_ARGS__)
#define ouster_log_m3(...) ouster_log_m3_(__VA_ARGS__)
#define ouster_log_v3(...) ouster_log_v3_(__VA_ARGS__)
#else
#define ouster_log(...)
#define ouster_log_m4(...)
#define ouster_log_m3(...)
#define ouster_log_v3(...)
#endif





#ifdef __cplusplus
}
#endif

#endif // OUSTER_LOG_H

/** @} */