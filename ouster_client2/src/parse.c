#include "parse.h"
#include "log.h"

#include <string.h>



#define OUSTER_LEGACY
#define FRAME_STATUS_THERMAL_SHUTDOWN_SHIFT 0
#define FRAME_STATUS_SHOT_LIMITING_SHIFT 4

void ouster_column_log(ouster_column_t const * column)
{
    ouster_log("ts=%ji, status=%ji, mid=%ji\n", (intmax_t)column->ts, (intmax_t)column->status, (intmax_t)column->mid);
}


void ouster_parse_column_element(char const * col, void * dst, int type, ouster_pf_t const * pf)
{
    int size = 0;
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
#ifdef OUSTER_LEGACY
    case ouster_id(ouster_frame_id_t):
        memcpy(dst, col + 10, sizeof(ouster_frame_id_t));
        break;
    }
#endif
}


void ouster_parse_column(char const * buf, int icol, ouster_pf_t const * pf, ouster_column_t * dst)
{
    char * col = buf + pf->packet_header_size + pf->col_size * icol;
    ouster_parse_column_element(col, &dst->ts, ouster_id(ouster_timestamp_t), pf);
    ouster_parse_column_element(col, &dst->status, ouster_id(ouster_status_t), pf);
    ouster_parse_column_element(col, &dst->mid, ouster_id(ouster_measurment_id_t), pf);
}






void ouster_lidar_packet_parse(char const * buf, ouster_pf_t const * pf, ouster_lidar_packet_t * dst)
{
    #ifdef OUSTER_LEGACY
    char * col = buf + pf->packet_header_size + pf->col_size * 0;
    ouster_parse_column_element(col, &(dst->frame_id), ouster_id(ouster_frame_id_t), pf);
    #elif
    memcpy(&(dst->frame_id), buf + 2, sizeof(ouster_frame_id_t));
    #endif
}


