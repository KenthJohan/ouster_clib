#pragma once

#include "jsmn.h"

typedef enum {
	JSON_TYPE_INT,
	JSON_TYPE_F64,
	JSON_TYPE_LAST
} json_type_t;

jsmntok_t *json_parse_value(char const *json, jsmntok_t *t, char const *path[], void *out, json_type_t type);
jsmntok_t *json_parse_vector(char const *json, jsmntok_t *t, char const *path[], void *out, int n, json_type_t type);
jsmntok_t *json_parse_string(char const *json, jsmntok_t *t, char const *path[], char *out, int n);
