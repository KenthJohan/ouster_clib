#pragma once


typedef struct
{
    int column_window[2];
    int columns_per_frame;
    int columns_per_packet;
    int pixels_per_column;
    char udp_profile_lidar[128];
} ouster_meta_t;


void ouster_meta_parse(char const * jsonstr, ouster_meta_t * out_meta);