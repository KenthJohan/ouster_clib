/**
 * @defgroup net Network and sockets
 * @brief Provides network functionality
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_NET_H
#define OUSTER_NET_H

#include <stdint.h>

// #define INET6_ADDRSTRLEN 46
#define OUSTER_NET_ADDRSTRLEN 46
#define OUSTER_NET_UDP_MAX_SIZE 65535
#define OUSTER_NET_FLAGS_NONBLOCK 0x0001
#define OUSTER_NET_FLAGS_REUSE 0x0002
#define OUSTER_NET_FLAGS_IPV6ONLY 0x0004
#define OUSTER_NET_FLAGS_IPV4 0x0008
#define OUSTER_NET_FLAGS_IPV6 0x0010
#define OUSTER_NET_FLAGS_UDP 0x0020
#define OUSTER_NET_FLAGS_TCP 0x0040
#define OUSTER_NET_FLAGS_BIND 0x0100
#define OUSTER_NET_FLAGS_CONNECT 0x0200

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int flags;
	int rcvbuf_size;
	int port;
	char const *hint_service;
	char const *hint_name;
	int rcvtimeout_sec;
	char const *group;
} ouster_net_sock_desc_t;

typedef struct
{
	char data[OUSTER_NET_ADDRSTRLEN];
} ouster_net_addr_t;

/** Set a IPv4 address
 *
 * @param addr The address
 * @param ip The IPv4 address
 */
void ouster_net_addr_set_ip4(ouster_net_addr_t *addr, char const *ip);

/** Set a port
 *
 * @param addr The address
 * @param ip The port
 */
void ouster_net_addr_set_port(ouster_net_addr_t *addr, int port);

/**
 * @brief Send data from a socket to a network address
 * 
 * @param sock The source socket filedescriptor
 * @param buf Data to send
 * @param size Data size to send
 * @param flags 
 * @param addr Destinationnetwork address
 * @return Returns the number sent, or -1 for errors.
 */
int ouster_net_sendto(int sock, char *buf, int size, int flags, ouster_net_addr_t *addr);

int ouster_net_create(ouster_net_sock_desc_t *desc);

int64_t ouster_net_read(int sock, char *buf, int len);

uint64_t ouster_net_select(int socks[], int n, const int timeout_sec, const int timeout_usec);

int32_t ouster_net_get_port(int sock);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_NET_H

/** @} */