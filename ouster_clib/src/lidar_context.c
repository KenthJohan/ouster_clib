#include "ouster_clib/lidar_context.h"
#include "ouster_clib/lidar_header.h"
#include "ouster_clib/lidar_column.h"
#include "ouster_clib/log.h"
#include <assert.h>
#include <string.h>

void pxcpy(char * dst, int dst_inc, char const * src, int src_inc, int n, int esize)
{
    assert(dst);
    assert(src);
    char * d = dst;
    char const * s = src;
    for (int i = 0; i < n; i++, d += dst_inc, s += src_inc)
    {
        memcpy(d, s, esize);
    }
}


void ouster_lidar_context_get_range(ouster_lidar_context_t * ctx, ouster_meta_t * meta, char const * buf, ouster_mat4_t * mat)
{
    assert(ctx);
    assert(buf);
    assert(mat);
    assert(meta->pixels_per_column == mat->dim[1]);
    
    char const * colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
    ouster_lidar_header_t header = {0};
    ouster_column_t column = {0};
    ouster_lidar_header_get(buf, &header);
    //ouster_lidar_header_log(&header);


    if (ctx->frame_id != (int)header.frame_id)
    {
        //ouster_log("New Frame!\n");
        ctx->frame_id = (int)header.frame_id;
        mat->num_valid_pixels = 0;
        ctx->last_mid = 0;
    }

    for(int icol = 0; icol < meta->columns_per_packet; icol++, colbuf += meta->col_size)
    {
        ouster_column_get(colbuf, &column);
        if((column.status & 0x01) == 0)
        {
            continue;
        }
        //ouster_column_log(&column);
        char const * pxbuf = colbuf + OUSTER_COLUMN_HEADER_SIZE;
        char * matdst = mat->data + (column.mid - meta->column_window[0]) * mat->step[1];

        // TODO: Copy rest of the fields also
        pxcpy(
            matdst, 
            mat->step[0], 
            pxbuf, 
            meta->channel_data_size, 
            mat->dim[1], 
            mat->step[0]
        );
        mat->num_valid_pixels += meta->pixels_per_column;
        ctx->last_mid = column.mid;
    }
}