#include "ouster_client2/meta.h"
#include "ouster_client2/log.h"
#include "jsmn.h"
#include <string.h>

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
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

#define TOK_COUNT 1024
void ouster_meta_parse(char const * jsonstr)
{
    jsmn_parser p;
    jsmn_init(&p);
    jsmntok_t t[TOK_COUNT];
    int r = jsmn_parse(&p, jsonstr, strlen(jsonstr), t, TOK_COUNT);
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
    for (int i = 1; i < r; i++)
    {
        if (jsoneq(jsonstr, &t[i], "client_version") == 0)
        {
            /* We may use strndup() to fetch string value */
            ouster_log("- client_version: %.*s\n", t[i + 1].end - t[i + 1].start, jsonstr + t[i + 1].start);
            i++;
        }
    }

}