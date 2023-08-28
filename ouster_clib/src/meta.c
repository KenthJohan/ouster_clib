#include "ouster_clib/meta.h"
#include "ouster_clib/log.h"
#include "ouster_clib/types.h"
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

void ouster_meta_parse(char const * json, ouster_meta_t * out)
{
    jsmn_parser p;
    jsmn_init(&p);
    jsmntok_t tokens[TOK_COUNT];
    int r = jsmn_parse(&p, json, strlen(json), tokens, TOK_COUNT);
    if (r < 1)
    {
        ouster_log("jsmn error: %s\n", jsmn_strerror(r));
        return;
    }
    assert(tokens[0].type == JSMN_OBJECT);

    json_parse_int(json, tokens, (char const *[]){"lidar_data_format", "columns_per_frame", NULL}, &out->columns_per_frame);
    json_parse_int(json, tokens, (char const *[]){"lidar_data_format", "columns_per_packet", NULL}, &out->columns_per_packet);
    json_parse_int(json, tokens, (char const *[]){"lidar_data_format", "pixels_per_column", NULL}, &out->pixels_per_column);
    json_parse_intv(json, tokens, (char const *[]){"lidar_data_format", "column_window", NULL}, out->column_window, 2);
    json_parse_string(json, tokens, (char const *[]){"lidar_data_format", "udp_profile_lidar", NULL}, out->udp_profile_lidar, 128);

    if(strcmp(out->udp_profile_lidar, "LIDAR_LEGACY") == 0)
    {
        out->channel_data_size = 12;
    }
    else if(strcmp(out->udp_profile_lidar, "RNG19_RFL8_SIG16_NIR16_DUAL") == 0)
    {
        out->channel_data_size = 16;
    }
    else if(strcmp(out->udp_profile_lidar, "RNG19_RFL8_SIG16_NIR16") == 0)
    {
        out->channel_data_size = 12;
    }
    else if(strcmp(out->udp_profile_lidar, "NG15_RFL8_NIR8") == 0)
    {
        out->channel_data_size = 4;
    }
    else if(strcmp(out->udp_profile_lidar, "FIVE_WORD_PIXEL") == 0)
    {
        out->channel_data_size = 20;
    }

/*
  https://github.com/ouster-lidar/ouster_example/blob/9d0971107f6f9c95e16afd727fa2534d01a0fe4e/ouster_client/src/parsing.cpp#L155

  col_size = col_header_size + pixels_per_column * channel_data_size +
              col_footer_size;

*/
    out->col_size = OUSTER_COLUMN_HEADER_SIZE + out->pixels_per_column * out->channel_data_size + OUSTER_COLUMN_FOOTER_SIZE;
}