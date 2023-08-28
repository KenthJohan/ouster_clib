#pragma once
#include "types.h"


typedef struct
{
    ouster_frame_id_t frame_id;
    ouster_packet_type_t packet_type;
    ouster_init_id_t init_id;
    ouster_prod_sn_t prod_sn;
    ouster_countdown_thermal_shutdown_t countdown_thermal_shutdown;
    ouster_countdown_shot_limiting_t countdown_shot_limiting;
    ouster_thermal_shutdown_t thermal_shutdown;
    ouster_shot_limiting_t shot_limiting;
} ouster_lidar_header_t;


void ouster_lidar_header_log(ouster_lidar_header_t * p);


void ouster_lidar_header_get(char const * buf, ouster_lidar_header_t * dst);