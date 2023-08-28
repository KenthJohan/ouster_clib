#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
    int column_window[2];
    int columns_per_frame;
    int columns_per_packet;
    int pixels_per_column;
    char udp_profile_lidar[128];

    int channel_data_size;
    int col_size;
} ouster_meta_t;


void ouster_meta_parse(char const * jsonstr, ouster_meta_t * out_meta);

#ifdef __cplusplus
}
#endif