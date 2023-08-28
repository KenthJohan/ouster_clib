#pragma once

#include "jsmn.h"




jsmntok_t * json_parse_int(char const * json, jsmntok_t * t, char const * path[], int * out);
jsmntok_t * json_parse_intv(char const * json, jsmntok_t * t, char const * path[], int * out, int n);
jsmntok_t * json_parse_string(char const * json, jsmntok_t * t, char const * path[], char * out, int n);
