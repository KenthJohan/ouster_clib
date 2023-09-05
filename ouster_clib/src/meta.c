#include "ouster_clib/meta.h"
#include "ouster_clib/types.h"

#include <platform/log.h>
#include <platform/basics.h>

#define JSMN_HEADER
#include "jsmn.h"
#include "json.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define TOK_COUNT 1024



char * jsmn_strerror(int r)
{
    switch (r)
    {
    case JSMN_ERROR_NOMEM: return "JSMN_ERROR_NOMEM";
    case JSMN_ERROR_INVAL: return "JSMN_ERROR_INVAL";
    case JSMN_ERROR_PART: return "JSMN_ERROR_PART";
    default: return "JSMN_ERROR_?";
    }
}
















#define STRING_BUF_SIZE 128


void ouster_meta_parse(char const * json, ouster_meta_t * out)
{
    assert(json);
    jsmn_parser p;
    jsmn_init(&p);
    jsmntok_t tokens[TOK_COUNT];
    int r = jsmn_parse(&p, json, strlen(json), tokens, TOK_COUNT);
    if (r < 1)
    {
        platform_log("jsmn error: %s\n", jsmn_strerror(r));
        return;
    }
    assert(tokens[0].type == JSMN_OBJECT);

    int column_window[2];
    json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "columns_per_frame", NULL}, &out->columns_per_frame, JSON_TYPE_INT);
    json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "columns_per_packet", NULL}, &out->columns_per_packet, JSON_TYPE_INT);
    json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "pixels_per_column", NULL}, &out->pixels_per_column, JSON_TYPE_INT);
    json_parse_vector(json, tokens, (char const *[]){"lidar_data_format", "column_window", NULL}, column_window, 2, JSON_TYPE_INT);
    assert(out->pixels_per_column > 0);
    assert(out->pixels_per_column <= 128);
    json_parse_vector(json, tokens, (char const *[]){"lidar_data_format", "pixel_shift_by_row", NULL}, out->pixel_shift_by_row, out->pixels_per_column, JSON_TYPE_INT);


    json_parse_value(json, tokens, (char const *[]){"beam_intrinsics", "lidar_origin_to_beam_origin_mm", NULL}, &out->lidar_origin_to_beam_origin_mm, JSON_TYPE_F64);
    json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_altitude_angles", NULL}, out->beam_altitude_angles, out->pixels_per_column, JSON_TYPE_F64);
    json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_azimuth_angles", NULL}, out->beam_azimuth_angles, out->pixels_per_column, JSON_TYPE_F64);
    json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_to_lidar_transform", NULL}, out->beam_to_lidar_transform, 16, JSON_TYPE_F64);

    json_parse_vector(json, tokens, (char const *[]){"lidar_intrinsics", "lidar_to_sensor_transform", NULL}, out->lidar_to_sensor_transform, 16, JSON_TYPE_F64);




    char buf[STRING_BUF_SIZE];
    json_parse_string(json, tokens, (char const *[]){"lidar_data_format", "udp_profile_lidar", NULL}, buf, STRING_BUF_SIZE);

    if(strcmp(buf, "LIDAR_LEGACY") == 0)
    {
        out->profile = OUSTER_PROFILE_LIDAR_LEGACY;
        out->channel_data_size = 12;
    }
    else if(strcmp(buf, "RNG19_RFL8_SIG16_NIR16_DUAL") == 0)
    {
        out->profile = OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL;
        out->channel_data_size = 16;
    }
    else if(strcmp(buf, "RNG19_RFL8_SIG16_NIR16") == 0)
    {
        out->profile = OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16;
        out->channel_data_size = 12;
    }
    else if(strcmp(buf, "NG15_RFL8_NIR8") == 0)
    {
        out->profile = OUSTER_PROFILE_RNG15_RFL8_NIR8;
        out->channel_data_size = 4;
    }
    else if(strcmp(buf, "FIVE_WORD_PIXEL") == 0)
    {
        out->profile = OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL;
        out->channel_data_size = 20;
    }


/*
  https://github.com/ouster-lidar/ouster_example/blob/9d0971107f6f9c95e16afd727fa2534d01a0fe4e/ouster_client/src/parsing.cpp#L155

  col_size = col_header_size + pixels_per_column * channel_data_size +
              col_footer_size;

        lidar_packet_size = packet_header_size + columns_per_packet * col_size +
                            packet_footer_size;

*/
    out->col_size = OUSTER_COLUMN_HEADER_SIZE + out->pixels_per_column * out->channel_data_size + OUSTER_COLUMN_FOOTER_SIZE;
    out->lidar_packet_size = OUSTER_PACKET_HEADER_SIZE + out->columns_per_packet * out->col_size + OUSTER_PACKET_FOOTER_SIZE;

    out->mid0 = MIN(column_window[0], column_window[1]);
    out->mid1 = MAX(column_window[0], column_window[1]);
    out->midw = abs(column_window[0] - column_window[1]) + 1;

    assert(out->mid0 < out->mid1);
    assert(out->midw > 0);
}