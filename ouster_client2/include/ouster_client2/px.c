#include "ouster_client2/px.h"
#include "ouster_client2/types.h"
#include <string.h>

void pxcpy(char * dst, int dst_stride, char * src, int src_stride, int n, int esize)
{
    char * d = dst;
    char * s = src;
    for (int i = 0; i < n; i++, d += dst_stride, s += src_stride)
    {
        memcpy(d, s, esize);
    }
}


typedef struct
{
    int bit_offset;
    int bit_size;
} px_desc_t;


px_desc_t px_descs[] = 
{
    [OUSTER_PROFILE_LIDAR_LEGACY] = (px_desc_t){.bit_offset = 0}
};



// https://static.ouster.dev/sensor-docs/image_route1/image_route2/sensor_data/sensor-data.html#single-return-profile
void ouster_px_copy___RNG19_RFL8_SIG16_NIR16(int format, int px_per_col)
{
    int src_stride = 12; // 96 bits
    int esize = 4;
    switch (format)
    {
    case ouster_id(ouster_field_range_t):
        break;
    }
    //pxcpy(dst, dst_stride, src, src_stride, px_per_col, esize);
}