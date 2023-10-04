#include "ouster_clib/client.h"

#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_log.h"

#include <assert.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

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