#include "ouster_client2/meta.h"
#include "ouster_client2/log.h"
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






void ouster_meta_parse(char const * json, ouster_meta_t * out_meta)
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

    json_parse_int(json, tokens, (char const *[]){"lidar_data_format", "columns_per_frame", NULL}, &out_meta->columns_per_frame);
    json_parse_int(json, tokens, (char const *[]){"lidar_data_format", "columns_per_packet", NULL}, &out_meta->columns_per_packet);
    json_parse_int(json, tokens, (char const *[]){"lidar_data_format", "pixels_per_column", NULL}, &out_meta->pixels_per_column);
    json_parse_intv(json, tokens, (char const *[]){"lidar_data_format", "column_window", NULL}, out_meta->column_window, 2);
    json_parse_string(json, tokens, (char const *[]){"lidar_data_format", "udp_profile_lidar", NULL}, out_meta->udp_profile_lidar, 128);


}