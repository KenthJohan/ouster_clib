#include "ouster_clib.h"
#pragma once

/*
 * MIT License
 *
 * Copyright (c) 2010 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef JSMN_H
#define JSMN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSMN_STATIC
#define JSMN_API static
#else
#define JSMN_API extern
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
  JSMN_UNDEFINED = 0,
  JSMN_OBJECT = 1 << 0,
  JSMN_ARRAY = 1 << 1,
  JSMN_STRING = 1 << 2,
  JSMN_PRIMITIVE = 1 << 3
} jsmntype_t;

enum jsmnerr {
  /* Not enough tokens were provided */
  JSMN_ERROR_NOMEM = -1,
  /* Invalid character inside JSON string */
  JSMN_ERROR_INVAL = -2,
  /* The string is not a full JSON packet, more bytes expected */
  JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct jsmntok {
  jsmntype_t type;
  int start;
  int end;
  int size;
#ifdef JSMN_PARENT_LINKS
  int parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */
typedef struct jsmn_parser {
  unsigned int pos;     /* offset in the JSON string */
  unsigned int toknext; /* next token to allocate */
  int toksuper;         /* superior token node, e.g. parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
JSMN_API void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each
 * describing
 * a single JSON object.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens);

#ifndef JSMN_HEADER
/**
 * Allocates a fresh unused token from the token pool.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens,
                                   const size_t num_tokens) {
  jsmntok_t *tok;
  if (parser->toknext >= num_tokens) {
    return NULL;
  }
  tok = &tokens[parser->toknext++];
  tok->start = tok->end = -1;
  tok->size = 0;
#ifdef JSMN_PARENT_LINKS
  tok->parent = -1;
#endif
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, const jsmntype_t type,
                            const int start, const int end) {
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
                                const size_t len, jsmntok_t *tokens,
                                const size_t num_tokens) {
  jsmntok_t *token;
  int start;

  start = parser->pos;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
#ifndef JSMN_STRICT
    /* In strict mode primitive must be followed by "," or "}" or "]" */
    case ':':
#endif
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ']':
    case '}':
      goto found;
    default:
                   /* to quiet a warning from gcc*/
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#ifdef JSMN_STRICT
  /* In strict mode primitive must be followed by a comma/object/array */
  parser->pos = start;
  return JSMN_ERROR_PART;
#endif

found:
  if (tokens == NULL) {
    parser->pos--;
    return 0;
  }
  token = jsmn_alloc_token(parser, tokens, num_tokens);
  if (token == NULL) {
    parser->pos = start;
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
#endif
  parser->pos--;
  return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
                             const size_t len, jsmntok_t *tokens,
                             const size_t num_tokens) {
  jsmntok_t *token;

  int start = parser->pos;
  
  /* Skip starting quote */
  parser->pos++;
  
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c = js[parser->pos];

    /* Quote: end of string */
    if (c == '\"') {
      if (tokens == NULL) {
        return 0;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
#ifdef JSMN_PARENT_LINKS
      token->parent = parser->toksuper;
#endif
      return 0;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->pos + 1 < len) {
      int i;
      parser->pos++;
      switch (js[parser->pos]) {
      /* Allowed escaped symbols */
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->pos++;
        for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0';
             i++) {
          /* If it isn't a hex character we have an error */
          if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) ||   /* 0-9 */
                (js[parser->pos] >= 65 && js[parser->pos] <= 70) ||   /* A-F */
                (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
            parser->pos = start;
            return JSMN_ERROR_INVAL;
          }
          parser->pos++;
        }
        parser->pos--;
        break;
      /* Unexpected symbol */
      default:
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens) {
  int r;
  int i;
  jsmntok_t *token;
  int count = parser->toknext;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c;
    jsmntype_t type;

    c = js[parser->pos];
    switch (c) {
    case '{':
    case '[':
      count++;
      if (tokens == NULL) {
        break;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        return JSMN_ERROR_NOMEM;
      }
      if (parser->toksuper != -1) {
        jsmntok_t *t = &tokens[parser->toksuper];
#ifdef JSMN_STRICT
        /* In strict mode an object or array can't become a key */
        if (t->type == JSMN_OBJECT) {
          return JSMN_ERROR_INVAL;
        }
#endif
        t->size++;
#ifdef JSMN_PARENT_LINKS
        token->parent = parser->toksuper;
#endif
      }
      token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
      token->start = parser->pos;
      parser->toksuper = parser->toknext - 1;
      break;
    case '}':
    case ']':
      if (tokens == NULL) {
        break;
      }
      type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
      if (parser->toknext < 1) {
        return JSMN_ERROR_INVAL;
      }
      token = &tokens[parser->toknext - 1];
      for (;;) {
        if (token->start != -1 && token->end == -1) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          token->end = parser->pos + 1;
          parser->toksuper = token->parent;
          break;
        }
        if (token->parent == -1) {
          if (token->type != type || parser->toksuper == -1) {
            return JSMN_ERROR_INVAL;
          }
          break;
        }
        token = &tokens[token->parent];
      }
#else
      for (i = parser->toknext - 1; i >= 0; i--) {
        token = &tokens[i];
        if (token->start != -1 && token->end == -1) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          parser->toksuper = -1;
          token->end = parser->pos + 1;
          break;
        }
      }
      /* Error if unmatched closing bracket */
      if (i == -1) {
        return JSMN_ERROR_INVAL;
      }
      for (; i >= 0; i--) {
        token = &tokens[i];
        if (token->start != -1 && token->end == -1) {
          parser->toksuper = i;
          break;
        }
      }
#endif
      break;
    case '\"':
      r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->toksuper != -1 && tokens != NULL) {
        tokens[parser->toksuper].size++;
      }
      break;
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;
    case ':':
      parser->toksuper = parser->toknext - 1;
      break;
    case ',':
      if (tokens != NULL && parser->toksuper != -1 &&
          tokens[parser->toksuper].type != JSMN_ARRAY &&
          tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
        parser->toksuper = tokens[parser->toksuper].parent;
#else
        for (i = parser->toknext - 1; i >= 0; i--) {
          if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
            if (tokens[i].start != -1 && tokens[i].end == -1) {
              parser->toksuper = i;
              break;
            }
          }
        }
#endif
      }
      break;
#ifdef JSMN_STRICT
    /* In strict mode primitives are: numbers and booleans */
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 't':
    case 'f':
    case 'n':
      /* And they must not be keys of the object */
      if (tokens != NULL && parser->toksuper != -1) {
        const jsmntok_t *t = &tokens[parser->toksuper];
        if (t->type == JSMN_OBJECT ||
            (t->type == JSMN_STRING && t->size != 0)) {
          return JSMN_ERROR_INVAL;
        }
      }
#else
    /* In non-strict mode every unquoted value is a primitive */
    default:
#endif
      r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->toksuper != -1 && tokens != NULL) {
        tokens[parser->toksuper].size++;
      }
      break;

#ifdef JSMN_STRICT
    /* Unexpected char in strict mode */
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (tokens != NULL) {
    for (i = parser->toknext - 1; i >= 0; i--) {
      /* Unmatched opened object or array */
      if (tokens[i].start != -1 && tokens[i].end == -1) {
        return JSMN_ERROR_PART;
      }
    }
  }

  return count;
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API void jsmn_init(jsmn_parser *parser) {
  parser->pos = 0;
  parser->toknext = 0;
  parser->toksuper = -1;
}

#endif /* JSMN_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* JSMN_H */


typedef enum {
	JSON_TYPE_INT,
	JSON_TYPE_F64,
	JSON_TYPE_LAST
} json_type_t;

jsmntok_t *json_parse_value(char const *json, jsmntok_t *t, char const *path[], void *out, json_type_t type);
jsmntok_t *json_parse_vector(char const *json, jsmntok_t *t, char const *path[], void *out, int n, json_type_t type);
jsmntok_t *json_parse_string(char const *json, jsmntok_t *t, char const *path[], char *out, int n);

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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define COLOR_ASSERT "\033[38;2;255;50;50m" // Red
#define COLOR_FILE "\033[38;2;100;100;100m" // Grey
#define COLOR_LINE "\033[38;2;100;100;100m" // Grey
#define COLOR_EXPR "\033[38;2;220;150;220m" // Purple
#define COLOR_RST "\033[0m"



int ouster_assert_(
    const char *expr,
    const char *file,
    int32_t line,
    const char *fn,
    const char *fmt,
    ...)
{
	FILE * f = stderr;
	fprintf(f, COLOR_ASSERT"A "COLOR_RST);
	fprintf(f, COLOR_FILE"%s"COLOR_RST":"COLOR_LINE"%i"COLOR_RST" ", file, line);
	fprintf(f, COLOR_EXPR"(%s)"COLOR_RST" ", expr);
	va_list args;
	va_start(args, fmt);
	int r = vfprintf(f, fmt, args);
	va_end(args);
	fprintf(f,"\n");
	fflush(stdout);
	fflush(stderr);
	return r;
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <curl/curl.h>

void ouster_buffer_init(ouster_buffer_t *b, int cap)
{
	ouster_assert_notnull(b);
	b->size = 0;
	b->cap = cap;
	b->data = calloc(1, b->cap);
}

void ouster_buffer_fini(ouster_buffer_t *b)
{
	ouster_assert_notnull(b);
	free(b->data);
	b->size = 0;
	b->cap = 0;
	b->data = NULL;
}

size_t write_memory_callback(void *contents, size_t element_size, size_t elements_count, void *user_pointer)
{
	ouster_assert_notnull(contents);
	ouster_assert_notnull(user_pointer);

	size_t size_increment = element_size * elements_count;
	ouster_buffer_t *b = user_pointer;
	int new_size = b->size + size_increment;
	if (new_size > b->cap) {
		b->data = realloc(b->data, new_size);
		if (b->data == NULL) {
			ouster_log("realloc(): error\n");
			return 0;
		}
		b->cap = new_size;
	}
	printf("contents:\n");
	fwrite(contents, size_increment, 1, stdout);
	printf("\n");
	memcpy(b->data + b->size, contents, size_increment);
	b->size += size_increment;
	return size_increment;
}

// http://192.168.1.137/api/v1/sensor/cmd/set_udp_dest_auto
// http://192.168.1.137/api/v1/sensor/cmd/set_udp_dest_auto
void req_get(ouster_client_t *client, char const *ip)
{
	ouster_assert_notnull(client);
	ouster_assert_notnull(ip);

	{
		char url[1024];
		snprintf(url, 1024, "http://%s/%s", client->host, ip);
		ouster_log("\033[4;34m"
		           "GET"
		           "\033[0m"
		           " %s\n",
		           url);
		curl_easy_setopt(client->curl, CURLOPT_URL, url);
	}

	curl_easy_setopt(client->curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

	client->buf.size = 0;
	curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, &client->buf);

	CURLcode res = curl_easy_perform(client->curl);
	ouster_log("curl_easy_perform() %i, %s\n", res, curl_easy_strerror(res));

	if (res == CURLE_SEND_ERROR) {
		// Specific versions of curl does't play well with the sensor http
		// server. When CURLE_SEND_ERROR happens for the first time silently
		// re-attempting the http request resolves the problem.
		res = curl_easy_perform(client->curl);
	}

	if (res != CURLE_OK) {
		ouster_log("curl_easy_perform() failed\n");
		return;
	}

	long http_code = 0;
	curl_easy_getinfo(client->curl, CURLINFO_RESPONSE_CODE, &http_code);

	if (http_code != 200) {
		ouster_log("http_code error: %i\n", http_code);
	}
}

#define URLAPI_METADATA "api/v1/sensor/metadata"
#define URLAPI_FIRMWARE "api/v1/system/firmware"

void ouster_client_init(ouster_client_t *client)
{
	ouster_assert_notnull(client);
	assert(client->host);
	curl_global_init(CURL_GLOBAL_ALL);
	client->curl = curl_easy_init();
	ouster_buffer_init(&client->buf, 1024 * 10);
	req_get(client, URLAPI_FIRMWARE);
}

void ouster_client_fini(ouster_client_t *client)
{
	ouster_assert_notnull(client);
	curl_easy_cleanup(client->curl);
	client->curl = NULL;
	ouster_buffer_fini(&client->buf);
}

void ouster_client_download_meta_file(ouster_client_t *client, char const *path)
{
	ouster_assert_notnull(client);
	ouster_assert_notnull(path);
	ouster_log("Downloading meta to %s\n", path);
	req_get(client, URLAPI_METADATA);
	FILE *f = fopen(path, "w+");
	fwrite(client->buf.data, client->buf.size, 1, f);
	fclose(f);
}

#include <string.h>

void ouster_dump_lidar_header(FILE *f, ouster_lidar_header_t const *p)
{
	ouster_assert_notnull(f);
	ouster_assert_notnull(p);
	fprintf(f, "type=%ji, frame=%ji, init=%ji, prod=%ji, countdown_thermal_shutdown=%ji, countdown_shot_limiting=%ji, thermal_shutdown=%ji, shot_limiting=%ji\n",
	        (intmax_t)p->packet_type,
	        (intmax_t)p->frame_id,
	        (intmax_t)p->init_id,
	        (intmax_t)p->prod_sn,
	        (intmax_t)p->countdown_thermal_shutdown,
	        (intmax_t)p->countdown_shot_limiting,
	        (intmax_t)p->thermal_shutdown,
	        (intmax_t)p->shot_limiting);
}

void ouster_dump_column(FILE *f, ouster_column_t const *column)
{
	ouster_assert_notnull(f);
	ouster_assert_notnull(column);
	fprintf(f, "ts=%ji, status=%ji, mid=%ji\n", (intmax_t)column->ts, (intmax_t)column->status, (intmax_t)column->mid);
}

void ouster_dump_meta(FILE * f, ouster_meta_t const *meta)
{
	ouster_assert_notnull(f);
	ouster_assert_notnull(meta);
	fprintf(f, "ouster_meta_t dump:\n");
	fprintf(f, "%40s: %i\n", "(Azimuth columns start) mid0", meta->mid0);
	fprintf(f, "%40s: %i\n", "(Azimuth columns end) mid1", meta->mid1);
	fprintf(f, "%40s: %i\n", "(Azimuth columns width) midw", meta->midw);
	fprintf(f, "%40s: %i\n", "udp_port_lidar", meta->udp_port_lidar);
	fprintf(f, "%40s: %i\n", "udp_port_imu", meta->udp_port_imu);
	fprintf(f, "%40s: %i\n", "columns_per_frame", meta->columns_per_frame);
	fprintf(f, "%40s: %i\n", "columns_per_packet", meta->columns_per_packet);
	fprintf(f, "%40s: %i\n", "pixels_per_column", meta->pixels_per_column);
	fprintf(f, "%40s: %iB\n", "channel_data_size", meta->channel_data_size);
	fprintf(f, "%40s: %i\n", "profile", meta->profile);
	fprintf(f, "%40s: %i\n", "channel_data_size", meta->channel_data_size);
	fprintf(f, "%40s: %iB\n", "col_size", meta->col_size);
	fprintf(f, "%40s: %iB\n", "lidar_packet_size", meta->lidar_packet_size);
} 
#include <stdlib.h>
#include <string.h>

void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t *meta)
{
	ouster_assert_notnull(fields);
	ouster_assert_notnull(meta);
	ouster_field_t *f = fields;
	for (int i = 0; i < count; ++i, f++) {
		ouster_assert(f->depth > 0, "");
		f->rows = meta->pixels_per_column;
		f->cols = meta->midw;
		f->rowsize = f->cols * f->depth;
		f->size = f->rows * f->cols * f->depth;
		f->data = calloc(1, fields->size);
	}
}

void ouster_field_cpy(ouster_field_t dst[], ouster_field_t src[], int count)
{
	ouster_assert_notnull(dst);
	ouster_assert_notnull(src);
	// memcpy(dst, src, sizeof(ouster_field_t) * count);
	for (int i = 0; i < count; ++i, ++dst, ++src) {
		memcpy(dst->data, src->data, src->size);
	}
}

/*
template <typename T>
inline img_t<T> destagger(const Eigen::Ref<const img_t<T>>& img,
                          const std::vector<int>& pixel_shift_by_row,
                          bool inverse) {
    const size_t h = img.rows();
    const size_t w = img.cols();

    if (pixel_shift_by_row.size() != h)
        throw std::invalid_argument{"image height does not match shifts size"};

    img_t<T> destaggered{h, w};
    for (size_t u = 0; u < h; u++) {
        const std::ptrdiff_t offset =
            ((inverse ? -1 : 1) * pixel_shift_by_row[u] + w) % w;

        destaggered.row(u).segment(offset, w - offset) =
            img.row(u).segment(0, w - offset);
        destaggered.row(u).segment(0, offset) =
            img.row(u).segment(w - offset, offset);
    }
    return destaggered;
}
*/

/*
https://static.ouster.dev/sdk-docs/reference/lidar-scan.html#staggering-and-destaggering
*/
void ouster_destagger(void *data, int cols, int rows, int depth, int rowsize, int pixel_shift_by_row[])
{
	ouster_assert_notnull(data);
	ouster_assert_notnull(pixel_shift_by_row);
	char *row = data;
	for (int irow = 0; irow < rows; ++irow, row += rowsize) {
		int offset = (pixel_shift_by_row[irow] + cols) % cols;
		memmove(row + depth * offset, row, depth * (cols - offset));
		memmove(row, row + depth * (cols - offset), depth * offset);
	}
}

void ouster_field_destagger(ouster_field_t fields[], int count, ouster_meta_t *meta)
{
	ouster_assert_notnull(fields);
	ouster_assert_notnull(meta);
	for (int i = 0; i < count; ++i, ++fields) {
		ouster_destagger(fields->data, fields->cols, fields->rows, fields->depth, fields->rowsize, meta->pixel_shift_by_row);
	}
}

void ouster_field_apply_mask_u32(ouster_field_t *field, ouster_meta_t *meta)
{
	ouster_assert_notnull(field);
	ouster_assert_notnull(meta);
	ouster_extract_t *extract = meta->extract + field->quantity;
	uint32_t mask = extract->mask;
	if (mask == 0xFFFFFFFF) {
		return;
	}

	ouster_assert(field->depth == 4, "Destination data depth other than 4 is not supported");
	if (field->depth == 4) {
		uint32_t *data32 = (uint32_t *)field->data;
		int rows = field->rows;
		int cols = field->cols;
		int cells = rows * cols;
		for (int i = 0; i < cells; ++i) {
			data32[i] &= mask;
		}
	}
}

void ouster_field_zero(ouster_field_t fields[], int count)
{
	ouster_assert_notnull(fields);
	for (int i = 0; i < count; ++i, ++fields) {
		memset(fields->data, 0, fields->size);
	}
}

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_FILENAME "\033[38;2;150;150;255m" // Blue
#define COLOR_CWD "\033[38;2;150;150;255m"      // Blue
#define COLOR_ERROR "\033[38;2;255;50;50m"      // Red
#define COLOR_RST "\033[0m"

int ouster_fs_readfile_failed_reason(char const *filename, char *buf, int len)
{
	char const *e = strerror(errno);
	char cwd[1024] = {0};
	char *rc = getcwd(cwd, sizeof(cwd));
	rc = rc ? rc : "";
	int n = snprintf(buf, len, "cwd:%s%s%s path:%s%s%s error:%s%s%s",
	                 COLOR_CWD, rc, COLOR_RST,
	                 COLOR_FILENAME, filename, COLOR_RST,
	                 COLOR_ERROR, e, COLOR_RST);
	return n;
}

void ouster_fs_pwd()
{
	char cwd[1024] = {0};
	char *rc = getcwd(cwd, sizeof(cwd));
	if (rc == NULL) {
		ouster_log("getcwd error: %s\n", strerror(errno));
		return;
	}
	printf("Current working directory: %s%s%s\n", COLOR_CWD, cwd, COLOR_RST);
}

char *ouster_fs_readfile(char const *path)
{
	ouster_assert_notnull(path);
	char *content = NULL;

	FILE *file = fopen(path, "r");
	if (file == NULL) {
		goto error;
	}

	fseek(file, 0, SEEK_END);
	int32_t size = (int32_t)ftell(file);

	if (size == -1) {
		goto error;
	}
	rewind(file);

	content = malloc(size + 1);
	content[size] = '\0';
	size_t n = fread(content, size, 1, file);
	if (n != 1) {
		ouster_log("%s: could not read wholef file %d bytes\n", path, size);
		goto error;
	}
	ouster_assert(content[size] == '\0', "Expected null terminator");
	fclose(file);
	return content;
error:
	if (content) {
		free(content);
	}
	return NULL;
}
#include <string.h>


void ouster_lidar_header_get1(char const *buf, void *dst, int type)
{
	ouster_assert_notnull(buf);
	ouster_assert_notnull(dst);
	switch (type) {
	case ouster_id(ouster_packet_type_t):
		memcpy(dst, buf + 0, sizeof(ouster_packet_type_t));
		break;
	case ouster_id(ouster_frame_id_t):
		memcpy(dst, buf + 2, sizeof(ouster_frame_id_t));
		break;
	case ouster_id(ouster_init_id_t):
		memcpy(dst, buf + 4, sizeof(ouster_init_id_t));
		*((ouster_init_id_t *)dst) &= UINT32_C(0x00ffffff);
		break;
	case ouster_id(ouster_prod_sn_t):
		memcpy(dst, buf + 7, sizeof(ouster_prod_sn_t));
		*((ouster_prod_sn_t *)dst) &= UINT64_C(0x000000ffffffffff);
		break;
	case ouster_id(ouster_countdown_thermal_shutdown_t):
		memcpy(dst, buf + 16, sizeof(ouster_countdown_thermal_shutdown_t));
		break;
	case ouster_id(ouster_countdown_shot_limiting_t):
		memcpy(dst, buf + 17, sizeof(ouster_countdown_shot_limiting_t));
		break;
	case ouster_id(ouster_thermal_shutdown_t):
		memcpy(dst, buf + 18, sizeof(ouster_thermal_shutdown_t));
		break;
	case ouster_id(ouster_shot_limiting_t):
		memcpy(dst, buf + 19, sizeof(ouster_shot_limiting_t));
		break;
	default:
		ouster_assert(0, "");
		break;
	}
}

void ouster_lidar_header_get(char const *buf, ouster_lidar_header_t *dst)
{
	ouster_assert_notnull(buf);
	ouster_assert_notnull(dst);
	ouster_lidar_header_get1(buf, &dst->packet_type, ouster_id(ouster_packet_type_t));
	ouster_lidar_header_get1(buf, &dst->frame_id, ouster_id(ouster_frame_id_t));
	ouster_lidar_header_get1(buf, &dst->init_id, ouster_id(ouster_init_id_t));
	ouster_lidar_header_get1(buf, &dst->prod_sn, ouster_id(ouster_prod_sn_t));
	ouster_lidar_header_get1(buf, &dst->countdown_thermal_shutdown, ouster_id(ouster_countdown_thermal_shutdown_t));
	ouster_lidar_header_get1(buf, &dst->countdown_shot_limiting, ouster_id(ouster_countdown_shot_limiting_t));
	ouster_lidar_header_get1(buf, &dst->thermal_shutdown, ouster_id(ouster_thermal_shutdown_t));
	ouster_lidar_header_get1(buf, &dst->shot_limiting, ouster_id(ouster_shot_limiting_t));
}



void ouster_column_get1(char const *colbuf, void *dst, int type)
{
	ouster_assert_notnull(colbuf);
	ouster_assert_notnull(dst);
	switch (type) {
	case ouster_id(ouster_timestamp_t):
		memcpy(dst, colbuf + 0, sizeof(ouster_timestamp_t));
		break;
	case ouster_id(ouster_measurment_id_t):
		memcpy(dst, colbuf + 8, sizeof(ouster_measurment_id_t));
		break;
	case ouster_id(ouster_status_t):
		memcpy(dst, colbuf + 10, sizeof(ouster_status_t));
		*((ouster_status_t *)dst) &= 0xffff;
		break;
	default:
		ouster_assert(0, "");
		break;
	}
}

void ouster_column_get(char const *colbuf, ouster_column_t *dst)
{
	ouster_assert_notnull(colbuf);
	ouster_assert_notnull(dst);
	ouster_column_get1(colbuf, &dst->ts, ouster_id(ouster_timestamp_t));
	ouster_column_get1(colbuf, &dst->status, ouster_id(ouster_status_t));
	ouster_column_get1(colbuf, &dst->mid, ouster_id(ouster_measurment_id_t));
}




void pxcpy(char *dst, int dst_inc, char const *src, int src_inc, int n, int esize)
{
	ouster_assert_notnull(dst);
	ouster_assert_notnull(src);
	char *d = dst;
	char const *s = src;
	for (int i = 0; i < n; i++, d += dst_inc, s += src_inc) {
		memcpy(d, s, esize);
	}
}

void field_copy(ouster_field_t *field, ouster_meta_t *meta, int mid, char const *pxbuf)
{
	ouster_assert_notnull(field);
	ouster_assert_notnull(meta);
	ouster_assert_notnull(pxbuf);
	// Row major - each row is continuous memory
	char *data = field->data;
	ouster_extract_t *extract = meta->extract + field->quantity;
	int depth = extract->depth;
	int offset = extract->offset;
	int rowsize = field->rowsize;
	int rows = meta->pixels_per_column;
	int mid0 = meta->mid0;
	int mid1 = meta->mid1;
	ouster_assert(mid <= mid1, "Incorrect mid=%i (Column Measurement Id) not in range %i to %i", mid, mid0, mid1);
	char *dst = data + (mid - mid0) * field->depth;
	pxcpy(dst, rowsize, pxbuf + offset, meta->channel_data_size, rows, depth);
}

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount)
{
	ouster_assert_notnull(lidar);
	ouster_assert_notnull(buf);
	ouster_assert(fields || ((fields == NULL) && (fcount == 0)), "");

	char const *colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
	ouster_lidar_header_t header = {0};
	ouster_column_t column = {0};
	ouster_lidar_header_get(buf, &header);
	// ouster_lidar_header_dump(&header);
	ouster_column_get(colbuf, &column);

	if (lidar->frame_id != (int)header.frame_id) {
		// ouster_log("New Frame!\n");
		lidar->frame_id = (int)header.frame_id;
		// lidar->last_mid = 0;
		lidar->last_mid = column.mid - 1;
		lidar->mid_loss = 0;
		lidar->num_valid_pixels = 0;
	}

	int mid_delta = column.mid - lidar->last_mid;
	//ouster_log("mid_delta %i\n", mid_delta);
	lidar->mid_loss += (mid_delta - 1);

	// col_size = 1584
	for (int icol = 0; icol < meta->columns_per_packet; icol++, colbuf += meta->col_size) {
		ouster_column_get(colbuf, &column);
		//ouster_column_dump(&column);

		if ((column.status & 0x01) == 0) {
			continue;
		}
		char const *pxbuf = colbuf + OUSTER_COLUMN_HEADER_SIZE;

		for (int j = 0; j < fcount; ++j) {
			ouster_assert(fields[j].cols > 0, "");
			ouster_assert(fields[j].rows > 0, "");
			ouster_assert(fields[j].depth > 0, "");
			field_copy(fields + j, meta, column.mid, pxbuf);
			lidar->num_valid_pixels += meta->pixels_per_column;
		}
		lidar->last_mid = column.mid;
	}

	for (int j = 0; j < fcount; ++j) {
		// TODO: Refactor this mask procedure
		ouster_field_apply_mask_u32(fields + j, meta);
	}

	for (int j = 0; j < fcount; ++j) {
		ouster_assert_notnull(fields[j].data);
		ouster_assert(fields[j].cols > 0, "");
		ouster_assert(fields[j].rows > 0, "");
		ouster_assert(fields[j].depth > 0, "");
	}
}
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void ouster_log_(char const *format, ...)
{
	assert(format);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
#pragma once

#define M4(i, j) ((i) * 4 + (j))
#define M3(i, j) ((i) * 3 + (j))
void mul3(double *r, double const *a, double const *x);

void m4_print(double const *a);

void m3_print(double const *a);

void v3_print(double const *a);


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*


XYZLut make_xyz_lut(size_t w, size_t h, double range_unit,
                    const mat4d& beam_to_lidar_transform,
                    const mat4d& transform,
                    const std::vector<double>& azimuth_angles_deg,
                    const std::vector<double>& altitude_angles_deg) {
    if (w <= 0 || h <= 0)
        throw std::invalid_argument("lut dimensions must be greater than zero");

    if ((azimuth_angles_deg.size() != h || altitude_angles_deg.size() != h) &&
        (azimuth_angles_deg.size() != w * h ||
         altitude_angles_deg.size() != w * h)) {
        throw std::invalid_argument("unexpected scan dimensions");
    }

    double beam_to_lidar_euclidean_distance_mm = beam_to_lidar_transform(0, 3);
    if (beam_to_lidar_transform(2, 3) != 0) {
        beam_to_lidar_euclidean_distance_mm =
            std::sqrt(std::pow(beam_to_lidar_transform(0, 3), 2) +
                      std::pow(beam_to_lidar_transform(2, 3), 2));
    }

    XYZLut lut;

    Eigen::ArrayXd encoder(w * h);   // theta_e
    Eigen::ArrayXd azimuth(w * h);   // theta_a
    Eigen::ArrayXd altitude(w * h);  // phi

    if (azimuth_angles_deg.size() == h && altitude_angles_deg.size() == h) {
        // OS sensor
        const double azimuth_radians = M_PI * 2.0 / w;

        // populate angles for each pixel
        for (size_t v = 0; v < w; v++) {
            for (size_t u = 0; u < h; u++) {
                size_t i = u * w + v;
                encoder(i) = 2.0 * M_PI - (v * azimuth_radians);
                azimuth(i) = -azimuth_angles_deg[u] * M_PI / 180.0;
                altitude(i) = altitude_angles_deg[u] * M_PI / 180.0;
            }
        }

    } else if (azimuth_angles_deg.size() == w * h &&
               altitude_angles_deg.size() == w * h) {
        // DF sensor
        // populate angles for each pixel
        for (size_t v = 0; v < w; v++) {
            for (size_t u = 0; u < h; u++) {
                size_t i = u * w + v;
                encoder(i) = 0;
                azimuth(i) = azimuth_angles_deg[i] * M_PI / 180.0;
                altitude(i) = altitude_angles_deg[i] * M_PI / 180.0;
            }
        }
    }

    // unit vectors for each pixel
    lut.direction = LidarScan::Points{w * h, 3};
    lut.direction.col(0) = (encoder + azimuth).cos() * altitude.cos();
    lut.direction.col(1) = (encoder + azimuth).sin() * altitude.cos();
    lut.direction.col(2) = altitude.sin();

    // offsets due to beam origin
    lut.offset = LidarScan::Points{w * h, 3};
    lut.offset.col(0) =
        encoder.cos() * beam_to_lidar_transform(0, 3) -
        lut.direction.col(0) * beam_to_lidar_euclidean_distance_mm;
    lut.offset.col(1) =
        encoder.sin() * beam_to_lidar_transform(0, 3) -
        lut.direction.col(1) * beam_to_lidar_euclidean_distance_mm;
    lut.offset.col(2) =
        -lut.direction.col(2) * beam_to_lidar_euclidean_distance_mm +
        beam_to_lidar_transform(2, 3);

    // apply the supplied transform
    auto rot = transform.topLeftCorner(3, 3).transpose();
    auto trans = transform.topRightCorner(3, 1).transpose();
    lut.direction.matrix() *= rot;
    lut.offset.matrix() *= rot;
    lut.offset.matrix() += trans.replicate(w * h, 1);

    // apply scaling factor
    lut.direction *= range_unit;
    lut.offset *= range_unit;

    return lut;
}
*/



void ouster_lut_fini(ouster_lut_t *lut)
{
	free(lut->direction);
	free(lut->offset);
	lut->direction = NULL;
	lut->offset = NULL;
}

/*
https://static.ouster.dev/sensor-docs/image_route1/image_route3/sensor_data/sensor-data.html?highlight=cartesian#lidar-range-to-xyz

*/
void ouster_lut_init(ouster_lut_t *lut, ouster_meta_t const *meta)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(meta);

	int w = meta->midw;
	int h = meta->pixels_per_column;
	ouster_assert(w >= 0, "");
	ouster_assert(w <= 1024, "");
	ouster_assert(h >= 0, "");
	ouster_assert(h <= 128, "");

	double *encoder = calloc(1, w * h * sizeof(double));  // theta_e
	double *azimuth = calloc(1, w * h * sizeof(double));  // theta_a
	double *altitude = calloc(1, w * h * sizeof(double)); // phi
	double *direction = calloc(1, w * h * 3 * sizeof(double));
	double *offset = calloc(1, w * h * 3 * sizeof(double));
	ouster_assert_notnull(encoder);
	ouster_assert_notnull(azimuth);
	ouster_assert_notnull(altitude);
	ouster_assert_notnull(direction);
	ouster_assert_notnull(offset);

	float beam_to_lidar_transform_03 = meta->beam_to_lidar_transform[M4(0, 3)];
	float beam_to_lidar_transform_23 = meta->beam_to_lidar_transform[M4(2, 3)];
	m4_print(meta->lidar_to_sensor_transform);

	// This represent a column measurement angle:
	double azimuth_radians = M_PI * 2.0 / meta->columns_per_frame;

	// OS sensor - populate angles for each pixel
	for (int c = 0; c < w; c++) {
		for (int r = 0; r < h; r++) {
			// Row major - each row is continuous memory
			int mid = meta->mid0 + c;
			ouster_assert(mid >= meta->mid0, "");
			ouster_assert(mid <= meta->mid1, "");
			int i = r * w + c;
			encoder[i] = 2.0 * M_PI - (mid * azimuth_radians);
			azimuth[i] = -meta->beam_azimuth_angles[r] * M_PI / 180.0;
			altitude[i] = meta->beam_altitude_angles[r] * M_PI / 180.0;
		}
	}

	// unit vectors for each pixel
	for (int i = 0; i < w * h; i++) {
		direction[i * 3 + 0] = cos(encoder[i] + azimuth[i]) * cos(altitude[i]);
		direction[i * 3 + 1] = sin(encoder[i] + azimuth[i]) * cos(altitude[i]);
		direction[i * 3 + 2] = sin(altitude[i]);
	}

	for (int i = 0; i < w * h; i++) {
		offset[i * 3 + 0] = cos(encoder[i]) * beam_to_lidar_transform_03 - direction[i * 3 + 0] * meta->lidar_origin_to_beam_origin_mm;
		offset[i * 3 + 1] = sin(encoder[i]) * beam_to_lidar_transform_03 - direction[i * 3 + 1] * meta->lidar_origin_to_beam_origin_mm;
		offset[i * 3 + 2] = beam_to_lidar_transform_23 - direction[i * 3 + 2] * meta->lidar_origin_to_beam_origin_mm;
	}

	// Extract the rotation matrix from transform
	double rotation[9] = {
	    [M3(0, 0)] = meta->lidar_to_sensor_transform[M4(0, 0)],
	    [M3(1, 0)] = meta->lidar_to_sensor_transform[M4(0, 1)],
	    [M3(2, 0)] = meta->lidar_to_sensor_transform[M4(0, 2)],
	    [M3(0, 1)] = meta->lidar_to_sensor_transform[M4(1, 0)],
	    [M3(1, 1)] = meta->lidar_to_sensor_transform[M4(1, 1)],
	    [M3(2, 1)] = meta->lidar_to_sensor_transform[M4(1, 2)],
	    [M3(0, 2)] = meta->lidar_to_sensor_transform[M4(2, 0)],
	    [M3(1, 2)] = meta->lidar_to_sensor_transform[M4(2, 1)],
	    [M3(2, 2)] = meta->lidar_to_sensor_transform[M4(2, 2)],
	};

	// Extract the translation vector from transform
	float translation[3] =
	    {
	        meta->lidar_to_sensor_transform[0 * 4 + 3],
	        meta->lidar_to_sensor_transform[1 * 4 + 3],
	        meta->lidar_to_sensor_transform[2 * 4 + 3],
	    };

	for (int i = 0; i < w * h; ++i) {
		double *d = direction + i * 3;
		double *o = offset + i * 3;
		mul3(d, rotation, d);
		mul3(o, rotation, o);
		o[0] += translation[0];
		o[1] += translation[1];
		o[2] += translation[2];
	}

	free(encoder);
	free(azimuth);
	free(altitude);

	lut->direction = direction;
	lut->offset = offset;
	lut->w = w;
	lut->h = h;

	/*
	for (int i = 0; i < w * h; ++i) {
		double *d = direction + i * 3;
		double *o = offset + i * 3;
		printf("%+f %+f %+f %+f %+f %+f, %+f\n", o[0], o[1], o[2], d[0], d[1], d[2], sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]));
	}
	*/
}

void ouster_lut_cartesian_f64(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(range);
	ouster_assert_notnull(out);

	double const *d = lut->direction;
	double const *o = lut->offset;
	char *out8 = out;

	for (int i = 0; i < (lut->w * lut->h); ++i, out8 += out_stride, d += 3, o += 3) {
		double mag = range[i];
		double *outd = (double *)out8;
		outd[0] = (float)(mag * d[0] + o[0]);
		outd[1] = (float)(mag * d[1] + o[1]);
		outd[2] = (float)(mag * d[2] + o[2]);
		// printf("%+f %+f\n", mag, sqrt(V3_DOT(out_xyz, out_xyz)));
	}
}
void ouster_lut_cartesian_f32(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(range);
	ouster_assert_notnull(out);

	double const *d = lut->direction;
	double const *o = lut->offset;
	char *out8 = out;

	for (int i = 0; i < (lut->w * lut->h); ++i, out8 += out_stride, d += 3, o += 3) {
		double mag = range[i];
		float *outf = (float *)out8;
		outf[0] = (float)(mag * d[0] + o[0]);
		outf[1] = (float)(mag * d[1] + o[1]);
		outf[2] = (float)(mag * d[2] + o[2]);
		// printf("%+f %+f\n", mag, sqrt(V3_DOT(out_xyz, out_xyz)));
	}
}

double *ouster_lut_alloc(ouster_lut_t const *lut)
{
	ouster_assert_notnull(lut);

	int n = lut->w * lut->h;
	int size = n * sizeof(double) * 3;
	void *memory = calloc(1, size);
	return memory;
}
#include <stdio.h>

#define FMTF "%+20.10f"
#define FMTF3 FMTF FMTF FMTF "\n"
#define FMTF4 FMTF FMTF FMTF FMTF "\n"

#define M4_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], (x)[15]
#define M4_ARGS_2(x) (x)[0], (x)[4], (x)[8], (x)[12], (x)[1], (x)[5], (x)[9], (x)[13], (x)[2], (x)[6], (x)[10], (x)[14], (x)[3], (x)[7], (x)[11], (x)[15]
#define M4_FORMAT FMTF4 FMTF4 FMTF4 FMTF4

#define M3_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8]
#define M3_ARGS_2(x) (x)[0], (x)[3], (x)[6], (x)[1], (x)[4], (x)[7], (x)[2], (x)[5], (x)[8]
#define M3_FORMAT FMTF3 FMTF3 FMTF3

#define V3_ARGS(x) (x)[0], (x)[1], (x)[2]
#define V3_FORMAT "%f %f %f\n"
#define V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

void m4_print(double const *a)
{
	printf(M4_FORMAT, M4_ARGS_1(a));
}

void m3_print(double const *a)
{
	printf(M3_FORMAT, M3_ARGS_1(a));
}

void v3_print(double const *a)
{
	printf(V3_FORMAT, V3_ARGS(a));
}

void mul3(double *r, double const *a, double const *x)
{
	ouster_assert_notnull(r);
	ouster_assert_notnull(a);
	ouster_assert_notnull(x);
	double temp[3];
	temp[0] = (a[M3(0, 0)] * x[0]) + (a[M3(1, 0)] * x[1]) + (a[M3(2, 0)] * x[2]);
	temp[1] = (a[M3(0, 1)] * x[0]) + (a[M3(1, 1)] * x[1]) + (a[M3(2, 1)] * x[2]);
	temp[2] = (a[M3(0, 2)] * x[0]) + (a[M3(1, 2)] * x[1]) + (a[M3(2, 2)] * x[2]);
	r[0] = temp[0];
	r[1] = temp[1];
	r[2] = temp[2];
}

#define JSMN_HEADER
#include <stdlib.h>
#include <string.h>

#define TOK_COUNT 1024


#define MAX(a, b) (((a) > (b)) ? a : b)
#define MIN(a, b) (((a) < (b)) ? a : b)

char *jsmn_strerror(int r)
{
	switch (r) {
	case JSMN_ERROR_NOMEM:
		return "JSMN_ERROR_NOMEM";
	case JSMN_ERROR_INVAL:
		return "JSMN_ERROR_INVAL";
	case JSMN_ERROR_PART:
		return "JSMN_ERROR_PART";
	default:
		return "JSMN_ERROR_?";
	}
}

#define STRING_BUF_SIZE 128

/*

static const Table<ChanField, FieldInfo, 8> legacy_field_info{{
    {ChanField::RANGE, {UINT32, 0, 0x000fffff, 0}},
    {ChanField::FLAGS, {UINT8, 3, 0, 4}},
    {ChanField::REFLECTIVITY, {UINT16, 4, 0, 0}},
    {ChanField::SIGNAL, {UINT16, 6, 0, 0}},
    {ChanField::NEAR_IR, {UINT16, 8, 0, 0}},
    {ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
    {ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
    {ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 5> lb_field_info{{
    {ChanField::RANGE, {UINT16, 0, 0x7fff, -3}},
    {ChanField::FLAGS, {UINT8, 1, 0b10000000, 7}},
    {ChanField::REFLECTIVITY, {UINT8, 2, 0, 0}},
    {ChanField::NEAR_IR, {UINT8, 3, 0, -4}},
    {ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 13> dual_field_info{{
    {ChanField::RANGE, {UINT32, 0, 0x0007ffff, 0}},
    {ChanField::FLAGS, {UINT8, 2, 0b11111000, 3}},
    {ChanField::REFLECTIVITY, {UINT8, 3, 0, 0}},
    {ChanField::RANGE2, {UINT32, 4, 0x0007ffff, 0}},
    {ChanField::FLAGS2, {UINT8, 6, 0b11111000, 3}},
    {ChanField::REFLECTIVITY2, {UINT8, 7, 0, 0}},
    {ChanField::SIGNAL, {UINT16, 8, 0, 0}},
    {ChanField::SIGNAL2, {UINT16, 10, 0, 0}},
    {ChanField::NEAR_IR, {UINT16, 12, 0, 0}},
    {ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
    {ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
    {ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
    {ChanField::RAW32_WORD4, {UINT32, 12, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 8> single_field_info{{
    {ChanField::RANGE, {UINT32, 0, 0x0007ffff, 0}},
    {ChanField::FLAGS, {UINT8, 2, 0b11111000, 3}},
    {ChanField::REFLECTIVITY, {UINT8, 4, 0, 0}},
    {ChanField::SIGNAL, {UINT16, 6, 0, 0}},
    {ChanField::NEAR_IR, {UINT16, 8, 0, 0}},
    {ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
    {ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
    {ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 14> five_word_pixel_info{{
    {ChanField::RANGE, {UINT32, 0, 0x0007ffff, 0}},
    {ChanField::FLAGS, {UINT8, 2, 0b11111000, 3}},
    {ChanField::REFLECTIVITY, {UINT8, 3, 0, 0}},
    {ChanField::RANGE2, {UINT32, 4, 0x0007ffff, 0}},
    {ChanField::FLAGS2, {UINT8, 6, 0b11111000, 3}},
    {ChanField::REFLECTIVITY2, {UINT8, 7, 0, 0}},
    {ChanField::SIGNAL, {UINT16, 8, 0, 0}},
    {ChanField::SIGNAL2, {UINT16, 10, 0, 0}},
    {ChanField::NEAR_IR, {UINT16, 12, 0, 0}},
    {ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
    {ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
    {ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
    {ChanField::RAW32_WORD4, {UINT32, 12, 0, 0}},
    {ChanField::RAW32_WORD5, {UINT32, 16, 0, 0}},
}};

*/

#define COMBINE(p, q) (((p) << 0) | ((q) << 8))

void ouster_extract_init(ouster_extract_t *f, ouster_profile_t profile, ouster_quantity_t quantity)
{
	ouster_assert_notnull(f);

	switch (COMBINE(profile, quantity)) {
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_RANGE):
		f->mask = UINT32_C(0x0007ffff);
		f->offset = 0;
		f->depth = 4;
		break;
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_REFLECTIVITY):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 4;
		f->depth = 1;
		break;
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_SIGNAL):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 6;
		f->depth = 2;
		break;
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_NEAR_IR):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 8;
		f->depth = 2;
		break;
		/*
	{ChanField::RANGE, {UINT16, 0, 0x7fff, -3}},
	{ChanField::FLAGS, {UINT8, 1, 0b10000000, 7}},
	{ChanField::REFLECTIVITY, {UINT8, 2, 0, 0}},
	{ChanField::NEAR_IR, {UINT8, 3, 0, -4}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
	*/
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_RANGE):
		f->mask = UINT32_C(0x7fff);
		f->offset = 0;
		f->depth = 2;
		break;
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_REFLECTIVITY):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 2;
		f->depth = 1;
		break;
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_NEAR_IR):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 3;
		f->depth = 1;
		break;
	default:
		break;
	}
}

void ouster_meta_parse(char const *json, ouster_meta_t *out)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(out);

	jsmn_parser p;
	jsmn_init(&p);
	jsmntok_t tokens[TOK_COUNT];
	int r = jsmn_parse(&p, json, strlen(json), tokens, TOK_COUNT);
	if (r < 1) {
		ouster_log("jsmn error: %s\n", jsmn_strerror(r));
		return;
	}
	ouster_assert(tokens[0].type == JSMN_OBJECT, "Expected JSMN_OBJECT");

	int column_window[2];
	json_parse_value(json, tokens, (char const *[]){"config_params", "udp_port_lidar", NULL}, &out->udp_port_lidar, JSON_TYPE_INT);
	json_parse_value(json, tokens, (char const *[]){"config_params", "udp_port_imu", NULL}, &out->udp_port_imu, JSON_TYPE_INT);
	json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "columns_per_frame", NULL}, &out->columns_per_frame, JSON_TYPE_INT);
	json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "columns_per_packet", NULL}, &out->columns_per_packet, JSON_TYPE_INT);
	json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "pixels_per_column", NULL}, &out->pixels_per_column, JSON_TYPE_INT);
	json_parse_vector(json, tokens, (char const *[]){"lidar_data_format", "column_window", NULL}, column_window, 2, JSON_TYPE_INT);
	ouster_assert(out->pixels_per_column > 0, "Not in range pixels_per_column");
	ouster_assert(out->pixels_per_column <= 128, "Not in range pixels_per_column");
	json_parse_vector(json, tokens, (char const *[]){"lidar_data_format", "pixel_shift_by_row", NULL}, out->pixel_shift_by_row, out->pixels_per_column, JSON_TYPE_INT);

	json_parse_value(json, tokens, (char const *[]){"beam_intrinsics", "lidar_origin_to_beam_origin_mm", NULL}, &out->lidar_origin_to_beam_origin_mm, JSON_TYPE_F64);
	json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_altitude_angles", NULL}, out->beam_altitude_angles, out->pixels_per_column, JSON_TYPE_F64);
	json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_azimuth_angles", NULL}, out->beam_azimuth_angles, out->pixels_per_column, JSON_TYPE_F64);
	json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_to_lidar_transform", NULL}, out->beam_to_lidar_transform, 16, JSON_TYPE_F64);

	json_parse_vector(json, tokens, (char const *[]){"lidar_intrinsics", "lidar_to_sensor_transform", NULL}, out->lidar_to_sensor_transform, 16, JSON_TYPE_F64);

	char buf[STRING_BUF_SIZE];
	json_parse_string(json, tokens, (char const *[]){"lidar_data_format", "udp_profile_lidar", NULL}, buf, STRING_BUF_SIZE);

	if (strcmp(buf, "LIDAR_LEGACY") == 0) {
		out->profile = OUSTER_PROFILE_LIDAR_LEGACY;
		out->channel_data_size = 12;
	} else if (strcmp(buf, "RNG19_RFL8_SIG16_NIR16_DUAL") == 0) {
		out->profile = OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL;
		out->channel_data_size = 16;
	} else if (strcmp(buf, "RNG19_RFL8_SIG16_NIR16") == 0) {
		out->profile = OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16;
		out->channel_data_size = 12;
	} else if (strcmp(buf, "RNG15_RFL8_NIR8") == 0) {
		out->profile = OUSTER_PROFILE_RNG15_RFL8_NIR8;
		out->channel_data_size = 4;
	} else if (strcmp(buf, "FIVE_WORD_PIXEL") == 0) {
		out->profile = OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL;
		out->channel_data_size = 20;
	} else {
		ouster_assert(0, "Profile not found");
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

	for (ouster_quantity_t quantity = 0; quantity < OUSTER_QUANTITY_CHAN_FIELD_MAX; ++quantity) {
		ouster_extract_t *e = out->extract + quantity;
		ouster_extract_init(e, out->profile, quantity);
		int rows = out->pixels_per_column;
		int cols = out->midw;
		ouster_assert(rows >= 0, "");
		ouster_assert(cols >= 0, "");
	}

	ouster_assert(out->mid0 < out->mid1, "");
	ouster_assert(out->midw > 0, "");
	ouster_assert(out->channel_data_size > 0, "");
}




#define _POSIX_C_SOURCE 200112L


#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

void ouster_net_addr_set_ip4(ouster_net_addr_t * addr, char const * ip)
{
	struct sockaddr_in * addr4 = (void*)addr;
	addr4->sin_family = AF_INET;
	addr4->sin_addr.s_addr = inet_addr(ip);
}

void net_addr_set_ip6(ouster_net_addr_t * addr, char const * ip)
{
	struct sockaddr_in6 * addr6 = (void*)addr;
	addr6->sin6_family = AF_INET6;
	// TODO: Set ip
}

void ouster_net_addr_set_port(ouster_net_addr_t * addr, int port)
{
	struct sockaddr * sa = (void*)addr;
	switch (sa->sa_family)
	{
	case AF_INET:{
		struct sockaddr_in * addr4 = (void*)addr;
		addr4->sin_port = htons(port);
		break;}
	
	case AF_INET6:{
		struct sockaddr_in6 * addr6 = (void*)addr;
		addr6->sin6_port = htons(port);
		break;}
	}
}

int ouster_net_sendto(int sock, char * buf, int size, int flags, ouster_net_addr_t * addr)
{
	struct sockaddr * sa = (void*)addr;
	ssize_t rc;
	switch (sa->sa_family)
	{
	case AF_INET:
		rc = sendto(sock, buf, size, flags, sa, sizeof(struct sockaddr_in));
		break;
	
	case AF_INET6:
		rc = sendto(sock, buf, size, flags, sa, sizeof(struct sockaddr_in6));
		break;
	}
	return rc;
}

int32_t ouster_net_get_port(int sock)
{
	ouster_assert(sock >= 0, "Socket not in range");

	struct sockaddr_storage ss;
	socklen_t addrlen = sizeof(ss);
	getsockname(sock, (struct sockaddr *)&ss, &addrlen);
	in_port_t port;
	switch (ss.ss_family) {
	case AF_INET:
		port = ((struct sockaddr_in *)&ss)->sin_port;
		break;
	case AF_INET6:
		port = ((struct sockaddr_in6 *)&ss)->sin6_port;
		break;
	}
	return ntohs(port);
}

void inet_ntop_addrinfo(struct addrinfo *ai, char *buf, socklen_t len)
{
	ouster_assert_notnull(ai);
	ouster_assert_notnull(buf);

	void *addr = NULL;
	switch (ai->ai_family) {
	case AF_INET:
		addr = &(((struct sockaddr_in *)ai->ai_addr)->sin_addr);
		break;
	case AF_INET6:
		addr = &(((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr);
		break;
	}
	inet_ntop(ai->ai_family, addr, buf, len);
}

int try_create_socket(ouster_net_sock_desc_t *desc, struct addrinfo *ai)
{
	ouster_assert_notnull(desc);
	ouster_assert_notnull(ai);

	int s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (s < 0) {
		ouster_log("socket(): error\n");
		goto error;
	}

	if (desc->flags & OUSTER_NET_FLAGS_CONNECT) {
		int rc = connect(s, ai->ai_addr, (socklen_t)ai->ai_addrlen);
		if (rc) {
			ouster_log("connect(): error: %s\n", strerror(errno));
			goto error;
		}
	}

	if (desc->rcvtimeout_sec) {
		struct timeval tv;
		tv.tv_sec = desc->rcvtimeout_sec;
		tv.tv_usec = 0;
		int rc = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
		if (rc == -1) {
			ouster_log("fcntl(): error\n");
			goto error;
		}
	}

	if (desc->flags & OUSTER_NET_FLAGS_IPV6ONLY) {
		int off = 0;
		int rc = setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off));
		if (rc) {
			ouster_log("setsockopt(): error\n");
			goto error;
		}
	}

	if (desc->flags & OUSTER_NET_FLAGS_REUSE) {
		int option = 1;
		int rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
		if (rc) {
			ouster_log("setsockopt(): error\n");
			goto error;
		}
	}

	if (desc->flags & OUSTER_NET_FLAGS_BIND) {

		int rc = bind(s, ai->ai_addr, (socklen_t)ai->ai_addrlen);
		if (rc) {
			ouster_log("bind(): error: %s\n", strerror(errno));
			goto error;
		}
	}

	// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6
	if (desc->group) {
#ifdef _GNU_SOURCE
		int rc;
		struct ip_mreq mreq; // IPv4
		rc = inet_pton(AF_INET, desc->group, &(mreq.imr_multiaddr.s_addr));
		if (rc != 1) {
			ouster_log("inet_pton(): error\n");
			goto error;
		}
		if (IN_MULTICAST(ntohl(mreq.imr_multiaddr.s_addr)) == 0) {
			ouster_log("Not multicast\n");
			goto error;
		}
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		rc = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
		if (rc < 0) {
			ouster_log("setsockopt(): error\n");
			goto error;
		}
		ouster_log("IP_ADD_MEMBERSHIP(): %s\n", desc->group);
#endif
		ouster_log("ip_mreq requires #define _GNU_SOURCE. Compile with -D_GNU_SOURCE or --std=gnu99\n");
	}

	if (desc->flags & OUSTER_NET_FLAGS_NONBLOCK) {
		int flags = fcntl(s, F_GETFL, 0);
		if (flags == -1) {
			ouster_log("fcntl(): error\n");
			goto error;
		}
		int rc = fcntl(s, F_SETFL, flags | O_NONBLOCK);
		if (rc == -1) {
			ouster_log("fcntl(): error\n");
			goto error;
		}
	}
	if (desc->rcvbuf_size) {
		int rc = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&desc->rcvbuf_size, sizeof(desc->rcvbuf_size));
		if (rc) {
			ouster_log("setsockopt(): error\n");
			goto error;
		}
	}
	return s;
error:
	if (s >= 0) {
		close(s);
	}
	return -1;
}

struct addrinfo *get_addrinfo(ouster_net_sock_desc_t *desc)
{
	ouster_assert_notnull(desc);

	char hint_service_buf[128];
	char const *hint_service = desc->hint_service;

	if (hint_service == NULL) {
		snprintf(hint_service_buf, 128, "%i", desc->port);
		hint_service = hint_service_buf;
	}

	struct addrinfo *info = NULL;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	if (desc->flags & OUSTER_NET_FLAGS_IPV4) {
		hints.ai_family = AF_INET;
	}
	if (desc->flags & OUSTER_NET_FLAGS_IPV6) {
		hints.ai_family = AF_INET6;
	}
	if (desc->flags & OUSTER_NET_FLAGS_UDP) {
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE;
	}
	if (desc->flags & OUSTER_NET_FLAGS_TCP) {
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_NUMERICHOST;
	}
	int ret = getaddrinfo(desc->hint_name, hint_service, &hints, &info);
	if (ret != 0) {
		hints.ai_flags = 0;
		ret = getaddrinfo(desc->hint_name, hint_service, &hints, &info);
		if (ret != 0) {
			ouster_log("getaddrinfo(): %s\n", gai_strerror(ret));
			goto error;
		}
	}
	if (info == NULL) {
		ouster_log("getaddrinfo(): empty result\n");
		goto error;
	}
	return info;

error:
	if (info) {
		ouster_log("freeaddrinfo()\n");
		freeaddrinfo(info);
	}
	return NULL;
}

int ouster_net_create(ouster_net_sock_desc_t *desc)
{
	ouster_assert_notnull(desc);

	struct addrinfo *info = get_addrinfo(desc);
	if (info == NULL) {
		ouster_log("get_addrinfo(): error\n");
		goto error;
	}
	struct addrinfo *ai;
	int s = -1;
	for (ai = info; ai != NULL; ai = ai->ai_next) {
		char buf[INET6_ADDRSTRLEN];
		inet_ntop_addrinfo(ai, buf, INET6_ADDRSTRLEN);
		ouster_log("get_addrinfo: %s\n", buf);
	}
	for (ai = info; ai != NULL; ai = ai->ai_next) {
		char buf[INET6_ADDRSTRLEN];
		inet_ntop_addrinfo(ai, buf, INET6_ADDRSTRLEN);
		s = try_create_socket(desc, ai);
		ouster_log("try_create_socket: %s:%i %s%s, socket=%i\n", buf, ouster_net_get_port(s),
		           (desc->flags & OUSTER_NET_FLAGS_TCP) ? "TCP" : "",
		           (desc->flags & OUSTER_NET_FLAGS_UDP) ? "UDP" : "",
		           s);
		if (s >= 0) {
			break;
		}
	}

	if (s < 0) {
		ouster_log("try_create_socket(): error\n");
		goto error;
	}

	freeaddrinfo(info);
	return s;

error:
	if (info) {
		ouster_log("freeaddrinfo()\n");
		freeaddrinfo(info);
	}
	if (s >= 0) {
		ouster_log("close() socket\n");
		close(s);
	}
	return -1;
}

int64_t ouster_net_read(int sock, char *buf, int len)
{
	ouster_assert(sock >= 0, "");
	ouster_assert_notnull(buf);

	int64_t bytes_read = recv(sock, (char *)buf, len, 0);
	return bytes_read;
}

int64_t net_write(int sock, char *buf, int len)
{
	ouster_assert(sock >= 0, "");
	ouster_assert_notnull(buf);

	int64_t bytes_read = recv(sock, (char *)buf, len, 0);
	return bytes_read;
}

/*
https://stackoverflow.com/questions/5647503/with-a-single-file-descriptor-is-there-any-performance-difference-between-selec
*/
uint64_t ouster_net_select(int socks[], int n, const int timeout_sec, const int timeout_usec)
{
	ouster_assert_notnull(socks);

	fd_set rfds;
	FD_ZERO(&rfds);
	for (int i = 0; i < n; ++i) {
		ouster_assert(socks[i] >= 0, "Socket not in range");
		FD_SET(socks[i], &rfds);
	}

	int max = 0;
	for (int i = 0; i < n; ++i) {
		if (socks[i] > max) {
			max = socks[i];
		}
	}

	uint64_t result = 0;
	struct timeval tv;
	tv.tv_sec = timeout_sec;
	tv.tv_usec = timeout_usec;

	int rc = select((int)max + 1, &rfds, NULL, NULL, &tv);
	if (rc == -1) {
		ouster_log("select(): error\n");
		return 0;
	}
	for (int i = 0; i < n; ++i) {
		if (FD_ISSET(socks[i], &rfds)) {
			result |= (1 << i);
		}
	}

	return result;
}

#include <stddef.h>

int ouster_sock_create_udp_lidar(int port)
{
	ouster_net_sock_desc_t desc = {0};
	desc.flags = OUSTER_NET_FLAGS_UDP | OUSTER_NET_FLAGS_NONBLOCK | OUSTER_NET_FLAGS_REUSE | OUSTER_NET_FLAGS_BIND;
	desc.hint_name = NULL;
	desc.rcvbuf_size = 1024 * 1024;
	desc.hint_service = NULL;
	desc.port = port;
	// desc.group = "239.201.201.201";
	// desc.group = "239.255.255.250";
	return ouster_net_create(&desc);
}

int ouster_sock_create_udp_imu(int port)
{
	ouster_net_sock_desc_t desc = {0};
	desc.flags = OUSTER_NET_FLAGS_UDP | OUSTER_NET_FLAGS_NONBLOCK | OUSTER_NET_FLAGS_REUSE | OUSTER_NET_FLAGS_BIND;
	desc.hint_name = NULL;
	desc.rcvbuf_size = 1024 * 1024;
	desc.hint_service = NULL;
	desc.port = port;
	return ouster_net_create(&desc);
}

int ouster_sock_create_tcp(char const *hint_name)
{
	ouster_assert_notnull(hint_name);

	ouster_net_sock_desc_t desc = {0};
	desc.flags = OUSTER_NET_FLAGS_TCP | OUSTER_NET_FLAGS_CONNECT;
	desc.hint_name = hint_name;
	desc.hint_service = "7501";
	desc.rcvtimeout_sec = 10;
	return ouster_net_create(&desc);
}

#define _DEFAULT_SOURCE
#include <endian.h>
#include <netinet/in.h>
#include <sys/socket.h>


/*
void ouster_ser_i32(char data[4], int32_t value)
{
    data[0] = ((uint32_t)value >> 24) & 0xFF;
    data[1] = ((uint32_t)value >> 16) & 0xFF;
    data[2] = ((uint32_t)value >> 8) & 0xFF;
    data[3] = ((uint32_t)value >> 0) & 0xFF;
}
*/

int ouster_udpcap_read(ouster_udpcap_t *cap, FILE *f)
{
	ouster_assert_notnull(cap);
	ouster_assert_notnull(f);

	{
		uint32_t maxsize = cap->size;
		// First get the header info
		size_t rc;
		rc = fread(cap, sizeof(ouster_udpcap_t), 1, f);
		if (rc != 1) {
			return OUSTER_UDPCAP_ERROR_FREAD;
		}
		// Convert to little endian to host
		cap->size = le32toh(cap->size);
		cap->port = le32toh(cap->port);
		if (cap->size <= maxsize) {
			return OUSTER_UDPCAP_ERROR_BUFFER_TOO_SMALL;
		}
	}

	{
		// Read the UDP content
		size_t rc;
		rc = fread(cap->buf, cap->size, 1, f);
		if (rc != 1) {
			return OUSTER_UDPCAP_ERROR_FREAD;
		}
	}

	return OUSTER_UDPCAP_OK;
}

int ouster_udpcap_sendto(ouster_udpcap_t *cap, int sock, ouster_net_addr_t *addr)
{
	ouster_assert_notnull(cap);
	ouster_assert(sock >= 0, "");
	ouster_assert_notnull(addr);

	ssize_t rc;
	// Send UDP content to the the same port as the capture
	ouster_net_addr_set_port(addr, cap->port);
	rc = ouster_net_sendto(sock, cap->buf, cap->size, 0, addr);
	// printf("rc %ji\n", (intmax_t)rc);
	return rc;
}

void ouster_udpcap_set_port(ouster_udpcap_t *cap, int port)
{
	ouster_assert_notnull(cap);

	cap->port = htole32(port);
}

int ouster_udpcap_sock_to_file(ouster_udpcap_t *cap, int sock, FILE *f)
{
	ouster_assert_notnull(cap);
	ouster_assert(sock >= 0, "");
	ouster_assert_notnull(f);

	int64_t write_size;

	{
		int64_t n = ouster_net_read(sock, cap->buf, cap->size);
		if (n <= 0) {
			return OUSTER_UDPCAP_ERROR_RECV;
		}
		if (n > cap->size) {
			return OUSTER_UDPCAP_ERROR_BUFFER_TOO_SMALL;
		}
		cap->size = htole32(n);
		write_size = sizeof(ouster_udpcap_t) + n;
	}

	{
		size_t n = fwrite((void *)cap, write_size, 1, f);
		if (n != 1) {
			return OUSTER_UDPCAP_ERROR_FWRITE;
		}
	}

	return OUSTER_UDPCAP_OK;
}
