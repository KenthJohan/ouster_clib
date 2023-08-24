#pragma once
#include <stdint.h>


#define ouster_id(T) OUSTER_ID##T##ID_

typedef float ouster_f32_t;
typedef double ouster_f64_t;
typedef uint64_t ouster_timestamp_t;
typedef uint16_t ouster_measurment_id_t;
typedef uint32_t ouster_status_t;
typedef uint16_t ouster_frame_id_t;
typedef uint64_t ouster_frame_status_t;
typedef uint16_t ouster_packet_type_t;
typedef uint32_t ouster_init_id_t;
typedef uint64_t ouster_prod_sn_t;
typedef uint8_t ouster_countdown_thermal_shutdown_t;
typedef uint8_t ouster_countdown_shot_limiting_t;
typedef uint8_t ouster_thermal_shutdown_t;
typedef uint8_t ouster_shot_limiting_t;

typedef enum
{
ouster_id(ouster_f32_t),
ouster_id(ouster_f64_t),
ouster_id(ouster_measurment_id_t),
ouster_id(ouster_timestamp_t),
ouster_id(ouster_status_t),
ouster_id(ouster_frame_id_t),
ouster_id(ouster_packet_type_t),
ouster_id(ouster_init_id_t),
ouster_id(ouster_prod_sn_t),
ouster_id(ouster_countdown_thermal_shutdown_t),
ouster_id(ouster_countdown_shot_limiting_t),
ouster_id(ouster_thermal_shutdown_t),
ouster_id(ouster_shot_limiting_t),
OUSTER_TYPE_LAST
} ouster_type_t;


typedef struct
{
    int packet_header_size;
    int col_header_size;
    int channel_data_size;
    int col_footer_size;
    int packet_footer_size;
    int col_size;
    int lidar_packet_size;
    int timestamp_offset;
    int measurement_id_offset;
    int status_offset;
} ouster_pf_t;



















