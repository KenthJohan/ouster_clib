#ifndef OUSTER_HTTP_H
#define OUSTER_HTTP_H
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OUSTER_HTTP_GET_METADATA "/api/v1/sensor/metadata"
#define OUSTER_HTTP_GET_FIRMWARE "/api/v1/system/firmware"

void ouster_http_request(int sock, char const *host, char const *path, ouster_vec_t *v);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_CLIENT_H