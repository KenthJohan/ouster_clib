#include "ouster_client2/lidar_context.h"
#include "ouster_client2/lidar_header.h"
#include "ouster_client2/lidar_column.h"
#include "ouster_client2/lidar_px.h"
#include "ouster_client2/log.h"



void lidar_context_get_range(lidar_context_t * ctx, char const * buf, ouster_mat_t * mat)
{
    char const * colbuf = buf + ctx->packet_header_size;
    ouster_lidar_header_t header;
    ouster_column_t column;
    ouster_lidar_header_get(buf, &header);
    //ouster_lidar_header_log(&header);

    /*
    Identify the maximum measurement id (mid) by storing the last mid as max mid
    when a new frame is recived:
    */ 
    if (ctx->frame_id != header.frame_id)
    {
        ctx->mid_max = ctx->mid_last;
        ctx->frame_id = header.frame_id;
        mat->num_valid_pixels = 0;
    }

    for(int icol = 0; icol < ctx->columns_per_packet; icol++, colbuf += ctx->col_size)
    {
        ouster_column_get(colbuf, &column);
        if((column.status & 0x01) == 0)
        {
            continue;
        }
        //ouster_column_log(&column);
        char const * pxbuf = colbuf + ctx->column_header_size;
        ouster_pxcpy(mat->data + icol * mat->stride, mat->esize, pxbuf, ctx->channel_data_size, ctx->pixels_per_column, mat->esize);
        mat->num_valid_pixels += ctx->pixels_per_column;
    }

    ctx->mid_last = column.mid;
    //ouster_log("max_mid=%i, last_mid=%i\n", ctx->max_mid, ctx->last_mid);
}