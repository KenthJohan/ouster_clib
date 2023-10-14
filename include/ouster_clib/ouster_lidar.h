#ifndef OUSTER_LIDAR_H
#define OUSTER_LIDAR_H

#include "ouster_clib.h"

#ifdef __cplusplus
extern "C"
{
#endif


void ouster_lidar_header_log(ouster_lidar_header_t *p);

void ouster_lidar_header_get(char const *buf, ouster_lidar_header_t *dst);

void ouster_column_get1(char const *colbuf, void *dst, int type);

void ouster_column_get(char const *colbuf, ouster_column_t *dst);

void ouster_column_log(ouster_column_t const *column);

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount);

#ifdef __cplusplus
}
#endif


#endif // OUSTER_LIDAR_H