#include "ouster_clib/lidar.h"
#include "ouster_clib/lidar_header.h"
#include "ouster_clib/lidar_column.h"
#include <platform/log.h>
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


void field_copy(ouster_field_t * field, ouster_meta_t * meta, int mid, char const * pxbuf)
{
    // Row major - each row is continuous memory
    char * data = field->mat.data;
    char * dst = data + (mid - meta->mid0) * field->mat.step[0];
    pxcpy(
        dst, 
        field->mat.step[1], 
        pxbuf + field->offset, 
        meta->channel_data_size, 
        field->mat.dim[2], 
        field->mat.step[0]
    );
}


void ouster_lidar_get_fields(ouster_lidar_t * lidar, ouster_meta_t * meta, char const * buf, ouster_field_t * fields, int fcount)
{
    assert(lidar);
    assert(buf);
    assert(fields);
    assert(meta->pixels_per_column == fields[0].mat.dim[2]);
    
    char const * colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
    ouster_lidar_header_t header = {0};
    ouster_column_t column = {0};
    ouster_lidar_header_get(buf, &header);
    //ouster_lidar_header_log(&header);
    ouster_column_get(colbuf, &column);


    if (lidar->frame_id != (int)header.frame_id)
    {
        //ouster_log("New Frame!\n");
        lidar->frame_id = (int)header.frame_id;
        //lidar->last_mid = 0;
        lidar->last_mid = column.mid - 1;
        lidar->mid_loss = 0;
        for(int j = 0; j < fcount; ++j)
        {
            fields[j].num_valid_pixels = 0;
        }
    }


    int mid_delta = column.mid - lidar->last_mid;
    //ouster_log("mid_delta %i\n", mid_delta);
    lidar->mid_loss += (mid_delta - 1);

    //col_size = 1584
    for(int icol = 0; icol < meta->columns_per_packet; icol++, colbuf += meta->col_size)
    {
        ouster_column_get(colbuf, &column);
        //ouster_column_log(&column);

        if((column.status & 0x01) == 0)
        {
            continue;
        }
        char const * pxbuf = colbuf + OUSTER_COLUMN_HEADER_SIZE;

        for(int j = 0; j < fcount; ++j)
        {
            // TODO: Copy rest of the fields also
            field_copy(fields + j, meta, column.mid, pxbuf);
            fields[j].num_valid_pixels += meta->pixels_per_column;
        }
        lidar->last_mid = column.mid;
    }
}