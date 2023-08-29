#include "ouster_clib/lidar.h"
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


void field_copy(ouster_field_t * field, ouster_meta_t * meta, int mid, char const * pxbuf)
{
    char * dst = field->mat.data + (mid - meta->column_window[0]) * field->mat.step[1];
    pxcpy(
        dst, 
        field->mat.step[0], 
        pxbuf + field->offset, 
        meta->channel_data_size, 
        field->mat.dim[1], 
        field->mat.step[0]
    );
}


void ouster_lidar_get_fields(ouster_lidar_t * lidar, ouster_meta_t * meta, char const * buf, ouster_field_t * fields, int fcount)
{
    assert(lidar);
    assert(buf);
    assert(fields);
    assert(meta->pixels_per_column == fields[0].mat.dim[1]);
    
    char const * colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
    ouster_lidar_header_t header = {0};
    ouster_column_t column = {0};
    ouster_lidar_header_get(buf, &header);
    //ouster_lidar_header_log(&header);


    if (lidar->frame_id != (int)header.frame_id)
    {
        //ouster_log("New Frame!\n");
        lidar->frame_id = (int)header.frame_id;
        lidar->last_mid = 0;
        for(int j = 0; j < fcount; ++j)
        {
            fields[j].num_valid_pixels = 0;
        }
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
        

        for(int j = 0; j < fcount; ++j)
        {
            // TODO: Copy rest of the fields also
            field_copy(fields + j, meta, column.mid, pxbuf);
            fields[j].num_valid_pixels += meta->pixels_per_column;
        }


        lidar->last_mid = column.mid;
    }
}