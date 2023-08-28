#include "ouster_client2/lidar_context.h"
#include "ouster_client2/lidar_header.h"
#include "ouster_client2/lidar_column.h"
#include "ouster_client2/log.h"
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


void lidar_context_get_range(lidar_context_t * ctx, char const * buf, ouster_mat4_t * mat)
{
    assert(ctx);
    assert(buf);
    assert(mat);
    assert(ctx->pixels_per_column == mat->dim[1]);
    
    char const * colbuf = buf + ctx->packet_header_size;
    ouster_lidar_header_t header = {0};
    ouster_column_t column = {0};
    ouster_lidar_header_get(buf, &header);
    //ouster_lidar_header_log(&header);

    /*
    Identify the maximum measurement id (mid) by storing the last mid as max mid
    when a new frame is recived:
    */ 
    if (ctx->frame_id != header.frame_id)
    {
        ouster_log("New Frame!\n");
        ouster_column_get(colbuf, &column);
        if((column.status & 0x01) == 0)
        {
            ouster_log("????????? %i\n", column.mid);
            //return;
        }
        /*
        if(column.mid < 740)
        {
            //ouster_log("?????????\n");
        }
        */
        ctx->mid_min = column.mid;
        ctx->mid_max = ctx->mid_last;
        ctx->frame_id = header.frame_id;
        mat->num_valid_pixels = 0;
    }

    // mid: 740 to 881
    for(int icol = 0; icol < ctx->columns_per_packet; icol++, colbuf += ctx->col_size)
    {
        ouster_column_get(colbuf, &column);
        if((column.status & 0x01) == 0)
        {
            continue;
        }
        ouster_column_log(&column);
        //char const * pxbuf = colbuf + ctx->column_header_size;
        //char * matdst = mat->data + column.mid * mat->step[1];

        // TODO: Copy rest of the fields also
        /*
        pxcpy(
            matdst, 
            mat->step[0], 
            pxbuf, 
            ctx->channel_data_size, 
            mat->dim[1], 
            mat->step[0]
        );
        */
        mat->num_valid_pixels += ctx->pixels_per_column;
        ctx->mid_last = column.mid;
    }

    
    ouster_log("mid_min=%i, mid_max=%i, mid_last=%i\n", ctx->mid_min, ctx->mid_max, ctx->mid_last);
}