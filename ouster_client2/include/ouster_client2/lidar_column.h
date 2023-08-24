#pragma once
#include "ouster_client2/types.h"

typedef struct
{
    ouster_timestamp_t ts;
    ouster_status_t status;
    ouster_measurment_id_t mid;
} ouster_column_t;


void ouster_column_get1(char const * col, void * dst, int type, ouster_pf_t const * pf);


void ouster_column_get(char const * buf, int icol, ouster_pf_t const * pf, ouster_column_t * dst);


void ouster_column_log(ouster_column_t const * column);