#pragma once
#include "ouster_clib/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define OUSTER_STRING_LENGTH 128

typedef struct
{
    int column_window[2];
    int columns_per_frame;
    int columns_per_packet;
    int pixels_per_column;
    ouster_profile_t profile;
    int channel_data_size;
    int col_size;
    int lidar_packet_size;
    int pixel_shift_by_row[OUSTER_STRING_LENGTH];


    int xxx_column_offset;
} ouster_meta_t;


void ouster_meta_parse(char const * jsonstr, ouster_meta_t * out_meta);
void ouster_meta_query3(int profile, int field, int attribute);

#ifdef __cplusplus
}
#endif