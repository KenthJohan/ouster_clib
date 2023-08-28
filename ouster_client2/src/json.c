#include "json.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void parse_int(char const * json, jsmntok_t * t, int * v)
{
    assert(json);
    assert(t);
    assert(v);
    assert(t->type == JSMN_PRIMITIVE);
    char buf[128] = {0};
    memcpy(buf, json + t->start, t->end - t->start);
    *v = atoi(buf);
}

jsmntok_t * parse_intv(char const * json, jsmntok_t * t, int v[], int n)
{
    assert(json);
    assert(t);
    assert(v);
    assert(t->type == JSMN_ARRAY);
    assert(t->size == 2);
    t++;
    for(int i = 0; i < n; ++i)
    {
        parse_int(json, t, v + i);
        t++;
    }
    return t;
}

void parse_string(char const * json, jsmntok_t * t, char buf[], int n)
{
    assert(json);
    assert(t);
    assert(buf);
    assert(t->type == JSMN_STRING);
    memcpy(buf, json + t->start, t->end - t->start);
    buf[t->end - t->start] = '\0';
}


int jsoneq(const char *json, jsmntok_t *t, const char *s)
{
    assert(json);
    assert(t);
    assert(s);
    if (
        (t->type == JSMN_STRING) && 
        ((int)strlen(s) == t->end - t->start) && 
        (strncmp(json + t->start, s, t->end - t->start) == 0)
    )
    {
        return 0;
    }
    return -1;
}

jsmntok_t * search(const char *json, jsmntok_t *t, const char *s)
{
    assert(json);
    assert(t);
    assert(s);
    while(1)
    {
        if(t->type == JSMN_UNDEFINED){return NULL;}
        if (jsoneq(json, t, s) == 0){return t;}
        t++;
    }
    return NULL;
}


jsmntok_t * search1(const char *json, jsmntok_t *t, char const * path[])
{
    assert(json);
    assert(t);
    assert(path);
    while(1)
    {
        if(t->type == JSMN_UNDEFINED){return NULL;}
        if(path[0] == NULL){return t;}
        t = search(json, t, path[0]);
        if(t == NULL){return NULL;}
        path++;
    }
    return t;
}


jsmntok_t * json_parse_int(char const * json, jsmntok_t * t, char const * path[], int * out)
{
    assert(json);
    assert(t);
    assert(path);
    assert(out);
    t = search1(json, t, path);
    if(t == NULL){return NULL;}
    parse_int(json, t+1, out);
    return t;
}

jsmntok_t * json_parse_intv(char const * json, jsmntok_t * t, char const * path[], int * out, int n)
{
    assert(json);
    assert(t);
    assert(path);
    assert(out);
    t = search1(json, t, path);
    if(t == NULL){return NULL;}
    t = parse_intv(json, t+1, out, n);
    return t;
}


jsmntok_t * json_parse_string(char const * json, jsmntok_t * t, char const * path[], char * out, int n)
{
    assert(json);
    assert(t);
    assert(path);
    assert(out);
    t = search1(json, t, path);
    if(t == NULL){return NULL;}
    parse_string(json, t+1, out, n);
    return t;
}