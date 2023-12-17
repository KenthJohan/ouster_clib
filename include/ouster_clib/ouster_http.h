/**
 * @defgroup http HTTP request
 * @brief Functionality for getting HTTP response from HTTP request
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_HTTP_H
#define OUSTER_HTTP_H

#include "ouster_clib/ouster_vec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OUSTER_HTTP_GET_METADATA "/api/v1/sensor/metadata"
#define OUSTER_HTTP_GET_FIRMWARE "/api/v1/system/firmware"

void ouster_http_request(int sock, char const *host, char const *path, ouster_vec_t *v);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_HTTP_H

/** @} */