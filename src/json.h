/**
 * @defgroup json JSON
 * @brief Functionality for json
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_JSON_H
#define OUSTER_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jsmn.h"

typedef enum {
	JSON_TYPE_INT,
	JSON_TYPE_F64,
	JSON_TYPE_LAST
} json_type_t;

jsmntok_t *json_parse_value(char const *json, jsmntok_t *t, char const *path[], void *out, json_type_t type);
jsmntok_t *json_parse_vector(char const *json, jsmntok_t *t, char const *path[], void *out, int n, json_type_t type);
jsmntok_t *json_parse_string(char const *json, jsmntok_t *t, char const *path[], char *out, int n);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_JSON_H

/** @} */