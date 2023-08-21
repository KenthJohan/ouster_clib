#pragma once

#include <stdint.h>
/*
#define ecs_set(world, entity, component, ...)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)
    */

#define ouster_id(T) OUSTER_ID##T##ID_
typedef float ouster_f32_t;
typedef double ouster_f64_t;
typedef uint16_t ouster_measurment_id_t;
typedef uint64_t ouster_timestamp_t;
typedef uint32_t ouster_status_t;
typedef uint16_t ouster_frame_id_t;
typedef uint64_t ouster_frame_status_t;

typedef enum
{
ouster_id(ouster_f32_t),
ouster_id(ouster_f64_t),
ouster_id(ouster_measurment_id_t),
ouster_id(ouster_timestamp_t),
ouster_id(ouster_status_t),
ouster_id(ouster_frame_id_t),
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

typedef struct
{
    ouster_timestamp_t ts;
    ouster_status_t status;
    ouster_measurment_id_t mid;
} ouster_column_t;


typedef struct
{
    ouster_frame_id_t frame_id;
    ouster_frame_status_t frame_status;
} ouster_lidar_packet_t;


void ouster_parse_column_element(char const * col, void * dst, int type, ouster_pf_t const * pf);

void ouster_parse_column(char const * buf, int icol, ouster_pf_t const * pf, ouster_column_t * dst);

void ouster_column_log(ouster_column_t const * column);

