#pragma once
#include "ouster_client2/types.h"


typedef struct 
{
    int packet_header_size;
    int columns_per_packet;
    int col_size;
    int column_header_size;
    int channel_data_size;
    int pixels_per_column;

    int frame_id;
    int mid_max;
    int mid_last;
} lidar_context_t;


void lidar_context_get_range(lidar_context_t * ctx, char const * buf, ouster_mat_t * mat);