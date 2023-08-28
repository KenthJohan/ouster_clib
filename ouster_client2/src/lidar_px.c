#include "ouster_client2/lidar_px.h"
#include <string.h>

void ouster_pxcpy(char * dst, int dst_inc, char const * src, int src_inc, int n, int esize)
{
    char * d = dst;
    char const * s = src;
    for (int i = 0; i < n; i++, d += dst_inc, s += src_inc)
    {
        memcpy(d, s, esize);
    }
}





