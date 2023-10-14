#include "json.h"
#include "ouster_clib/ouster_assert.h"
#include <stdlib.h>
#include <string.h>

int value_get_size(json_type_t type)
{
	switch (type) {
	case JSON_TYPE_INT:
		return sizeof(int);
	case JSON_TYPE_F64:
		return sizeof(double);
	default:
		return 0;
	}
}

void parse_value(char const *json, jsmntok_t *t, void *out, json_type_t type)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(out);
	ouster_assert(t->type == JSMN_PRIMITIVE, "Expected JSMN_PRIMITIVE");
	char buf[128] = {0};
	char *ptr;
	memcpy(buf, json + t->start, t->end - t->start);
	switch (type) {
	case JSON_TYPE_INT: {
		int value = atoi(buf);
		*((int *)out) = value;
		break;
	}
	case JSON_TYPE_F64: {
		double value = strtod(buf, &ptr);
		*((double *)out) = value;
		break;
	}
	default:
		break;
	}
}

jsmntok_t *parse_vector(char const *json, jsmntok_t *t, char *out, int n, json_type_t type)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(out);
	ouster_assert(t->type == JSMN_ARRAY, "Expected JSMN_ARRAY");
	ouster_assert(t->size == n, "");
	int esize = value_get_size(type);
	t++;
	for (int i = 0; i < n; ++i, out += esize) {
		parse_value(json, t, out, type);
		t++;
	}
	return t;
}

void parse_string(char const *json, jsmntok_t *t, char buf[], int n)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(buf);
	ouster_assert(t->type == JSMN_STRING, "Expected JSMN_STRING");
	memcpy(buf, json + t->start, t->end - t->start);
	buf[t->end - t->start] = '\0';
}

int jsoneq(const char *json, jsmntok_t *t, const char *s)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(s);
	if (
	    (t->type == JSMN_STRING) &&
	    ((int)strlen(s) == t->end - t->start) &&
	    (strncmp(json + t->start, s, t->end - t->start) == 0)) {
		return 0;
	}
	return -1;
}

jsmntok_t *search(const char *json, jsmntok_t *t, const char *s)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(s);
	while (1) {
		if (t->type == JSMN_UNDEFINED) {
			return NULL;
		}
		if (jsoneq(json, t, s) == 0) {
			return t;
		}
		t++;
	}
	return NULL;
}

jsmntok_t *search1(const char *json, jsmntok_t *t, char const *path[])
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(path);
	while (1) {
		if (t->type == JSMN_UNDEFINED) {
			return NULL;
		}
		if (path[0] == NULL) {
			return t;
		}
		t = search(json, t, path[0]);
		if (t == NULL) {
			return NULL;
		}
		path++;
	}
	return t;
}

jsmntok_t *json_parse_vector(char const *json, jsmntok_t *t, char const *path[], void *out, int n, json_type_t type)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(path);
	ouster_assert_notnull(out);
	t = search1(json, t, path);
	if (t == NULL) {
		return NULL;
	}
	t = parse_vector(json, t + 1, out, n, type);
	return t;
}

jsmntok_t *json_parse_value(char const *json, jsmntok_t *t, char const *path[], void *out, json_type_t type)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(path);
	ouster_assert_notnull(out);
	t = search1(json, t, path);
	if (t == NULL) {
		return NULL;
	}
	parse_value(json, t + 1, out, type);
	return t;
}

jsmntok_t *json_parse_string(char const *json, jsmntok_t *t, char const *path[], char *out, int n)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(t);
	ouster_assert_notnull(path);
	ouster_assert_notnull(out);
	t = search1(json, t, path);
	if (t == NULL) {
		return NULL;
	}
	parse_string(json, t + 1, out, n);
	return t;
}