/**
 * @defgroup dump Dumps structs
 * @brief Prints the content of ouster structs
 *
 * \ingroup c
 * @{
 */


#ifndef OUSTER_DUMP_H
#define OUSTER_DUMP_H

#include "ouster_clib.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void ouster_dump_lidar_header(FILE * f, ouster_lidar_header_t const *p);

void ouster_dump_column(FILE * f, ouster_column_t const *column);

void ouster_dump_meta(FILE *f, ouster_meta_t const *meta);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_DUMP_H

/** @} */