#pragma once
#include "ouster_clib/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ouster_timestamp_t ts;
    ouster_status_t status;
    ouster_measurment_id_t mid;
} ouster_column_t;


void ouster_column_get1(char const * colbuf, void * dst, int type);


void ouster_column_get(char const * colbuf, ouster_column_t * dst);


void ouster_column_log(ouster_column_t const * column);

#ifdef __cplusplus
}
#endif