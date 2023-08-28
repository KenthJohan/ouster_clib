#include "ouster_client2/lidar_column.h"
#include "ouster_client2/log.h"
#include <string.h>
#include <assert.h>


void ouster_column_log(ouster_column_t const * column)
{
    assert(column);
    ouster_log("ts=%ji, status=%ji, mid=%ji\n", (intmax_t)column->ts, (intmax_t)column->status, (intmax_t)column->mid);
}


void ouster_column_get1(char const * colbuf, void * dst, int type)
{
    assert(colbuf);
    assert(dst);
    switch (type)
    {
    case ouster_id(ouster_timestamp_t):
        memcpy(dst, colbuf + 0, sizeof(ouster_timestamp_t));
        break;
    case ouster_id(ouster_measurment_id_t):
        memcpy(dst, colbuf + 8, sizeof(ouster_measurment_id_t));
        break;
    case ouster_id(ouster_status_t):
        memcpy(dst, colbuf + 10, sizeof(ouster_status_t));
        *((ouster_status_t*)dst) &= 0xffff;
        break;
    default:
        assert(0);
        break;
    }
}


void ouster_column_get(char const * colbuf, ouster_column_t * dst)
{
    assert(colbuf);
    assert(dst);
    ouster_column_get1(colbuf, &dst->ts, ouster_id(ouster_timestamp_t));
    ouster_column_get1(colbuf, &dst->status, ouster_id(ouster_status_t));
    ouster_column_get1(colbuf, &dst->mid, ouster_id(ouster_measurment_id_t));
}


