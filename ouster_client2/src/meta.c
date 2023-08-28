#include "ouster_client2/meta.h"
#include "ouster_client2/log.h"
#include "jsmn.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define TOK_COUNT 1024

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (
        (tok->type == JSMN_STRING) && 
        ((int)strlen(s) == tok->end - tok->start) && 
        (strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    )
    {
        return 0;
    }
    return -1;
}

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


int parse_int(char const * jsonstr, jsmntok_t * t)
{
    assert(t->type == JSMN_PRIMITIVE);
    char buf[128] = {0};
    memcpy(buf, jsonstr + t->start, t->end - t->start);
    return atoi(buf);
}

jsmntok_t * parse_intv(char const * jsonstr, jsmntok_t * t, int v[], int n)
{
    assert(t->type == JSMN_ARRAY);
    assert(t->size == 2);
    t++;
    for(int i = 0; i < n; ++i)
    {
        v[i] = parse_int(jsonstr, t);
        t++;
    }
    return t;
}


jsmntok_t * parse_lidar_data_format(char const * jsonstr, jsmntok_t * t, ouster_meta_t * out_meta)
{
    assert(t->type == JSMN_OBJECT);
    t++;
    if (jsoneq(jsonstr, t, "column_window") == 0)
    {
        t++;
        t = parse_intv(jsonstr, t, out_meta->column_window, 2);
    }
    if (jsoneq(jsonstr, t, "columns_per_frame") == 0)
    {
        t++;
        out_meta->columns_per_frame = parse_int(jsonstr, t);
        t++;
    }
    if (jsoneq(jsonstr, t, "columns_per_packet") == 0)
    {
        t++;
        out_meta->columns_per_packet = parse_int(jsonstr, t);
        t++;
    }
    return t;
}

jsmntok_t * parse_root(char const * jsonstr, jsmntok_t t[], ouster_meta_t * out_meta)
{
    if (jsoneq(jsonstr, t, "client_version") == 0)
    {
        t++;
        assert(t->type == JSMN_STRING);
        ouster_log("- client_version: %.*s\n", t->end - t->start, jsonstr + t->start);
    }
    else if (jsoneq(jsonstr, t, "lidar_data_format") == 0)
    {
        t = parse_lidar_data_format(jsonstr, t + 1, out_meta);
    }
    return t + 1;
}





void ouster_meta_parse(char const * jsonstr, ouster_meta_t * out_meta)
{
    jsmn_parser p;
    jsmn_init(&p);
    jsmntok_t tokens[TOK_COUNT];
    jsmntok_t * t = tokens;
    int r = jsmn_parse(&p, jsonstr, strlen(jsonstr), tokens, TOK_COUNT);
    if (r < 1)
    {
        ouster_log("jsmn error: %s\n", jsmn_strerror(r));
        return;
    }
    if (t[0].type != JSMN_OBJECT)
    {
        ouster_log("Object expected: %i\n", t[0].type);
        return;
    }
    while(t < (tokens + TOK_COUNT))
    {
        t = parse_root(jsonstr, t, out_meta);
    }

}