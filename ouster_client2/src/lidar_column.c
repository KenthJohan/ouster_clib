#include "ouster_client2/lidar_column.h"
#include "ouster_client2/log.h"
#include <string.h>


void ouster_column_log(ouster_column_t const * column)
{
    ouster_log("ts=%ji, status=%ji, mid=%ji\n", (intmax_t)column->ts, (intmax_t)column->status, (intmax_t)column->mid);
}


void ouster_column_get1(char const * col, void * dst, int type, ouster_pf_t const * pf)
{
    switch (type)
    {
    case ouster_id(ouster_measurment_id_t):
        memcpy(dst, col + pf->measurement_id_offset, sizeof(ouster_measurment_id_t));
        break;
    case ouster_id(ouster_timestamp_t):
        memcpy(dst, col + pf->timestamp_offset, sizeof(ouster_timestamp_t));
        break;
    case ouster_id(ouster_status_t):
        memcpy(dst, col + pf->status_offset, sizeof(ouster_status_t));
        break;
    }
}


void ouster_column_get(char const * buf, int icol, ouster_pf_t const * pf, ouster_column_t * dst)
{
    char const * col = buf + pf->packet_header_size + pf->col_size * icol;
    ouster_column_get1(col, &dst->ts, ouster_id(ouster_timestamp_t), pf);
    ouster_column_get1(col, &dst->status, ouster_id(ouster_status_t), pf);
    ouster_column_get1(col, &dst->mid, ouster_id(ouster_measurment_id_t), pf);
}


