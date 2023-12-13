#ifndef OUSTER_CLIENT_H
#define OUSTER_CLIENT_H
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	char *data;
	int size;
	int cap;
} ouster_buffer_t;

typedef struct
{
	void *curl;
	char const *host;
	ouster_buffer_t buf;
} ouster_client_t;

void ouster_client_init(ouster_client_t *client);
void ouster_client_fini(ouster_client_t *client);
void ouster_client_download_meta_file(ouster_client_t *client, char const *path);
void ouster_client_write_meta_file(ouster_client_t *client, FILE * f);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_CLIENT_H