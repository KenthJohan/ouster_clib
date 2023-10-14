/**
 * @defgroup c_addons_monitor Monitor
 * @brief The monitor addon periodically tracks statistics for the world and systems.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef OUSTER_UDPCAP_H
#define OUSTER_UDPCAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "ouster_clib/ouster_net.h"

typedef struct
{
	uint32_t port;
	uint32_t size;
	char buf[];
} ouster_udpcap_t;


void ouster_udpcap_read(ouster_udpcap_t *cap, FILE *f);

int ouster_udpcap_sendto(ouster_udpcap_t *cap, int sock, ouster_net_addr_t *addr);

void ouster_udpcap_sock_to_file(ouster_udpcap_t *cap, int sock, FILE *f);

void ouster_udpcap_set_port(ouster_udpcap_t *cap, int port);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_UDPCAP_H

/** @} */