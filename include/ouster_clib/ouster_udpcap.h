/**
 * @defgroup udpcap UDP Capture/Replay
 * @brief Functionality for capturing and replaying UDP packets
 *
 * \ingroup c
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

/** Read file into capture buffer
 *
 * @param cap The capture buffer.
 * @param f Source file
 */
void ouster_udpcap_read(ouster_udpcap_t *cap, FILE *f);

/** Send the capture buffer from sock to addr
 *
 * @param cap The capture buffer.
 * @param sock The source socket filedescriptor
 * @param addr The destination address
 * @return Returns the number sent, or -1 for errors.
 */
int ouster_udpcap_sendto(ouster_udpcap_t *cap, int sock, ouster_net_addr_t *addr);

/** Find header in request.
 *
 * @param cap The capture buffer.
 * @param sock The socket filedescriptor
 * @param f Destination file
 * @return Returns the number sent, or -1 for errors.
 */
void ouster_udpcap_sock_to_file(ouster_udpcap_t *cap, int sock, FILE *f);

/** Set the UDP port of the capture buffer.
 *
 * @param cap The capture buffer.
 * @param port The port
 */
void ouster_udpcap_set_port(ouster_udpcap_t *cap, int port);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_UDPCAP_H

/** @} */