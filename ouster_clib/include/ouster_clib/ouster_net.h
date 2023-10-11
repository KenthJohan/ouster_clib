#pragma once
#include <stdint.h>

// #define INET6_ADDRSTRLEN 46
#define NET_ADDRSTRLEN 46

#define NET_UDP_MAX_SIZE 65535

#define NET_FLAGS_NONBLOCK 0x0001
#define NET_FLAGS_REUSE 0x0002
#define NET_FLAGS_IPV6ONLY 0x0004
#define NET_FLAGS_IPV4 0x0008
#define NET_FLAGS_IPV6 0x0010
#define NET_FLAGS_UDP 0x0020
#define NET_FLAGS_TCP 0x0040
#define NET_FLAGS_BIND 0x0100
#define NET_FLAGS_CONNECT 0x0200

#ifdef __cplusplus
extern "C"
{
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
} net_sock_desc_t;

typedef struct
{
	char data[NET_ADDRSTRLEN];
} net_addr_t;

void net_addr_set_ip4(net_addr_t * addr, char const * ip);
void net_addr_set_port(net_addr_t * addr, int port);
int net_sendto(int sock, char * buf, int size, int flags, net_addr_t * addr);

int net_create(net_sock_desc_t *desc);

int64_t net_read(int sock, char *buf, int len);

uint64_t net_select(int socks[], int n, const int timeout_sec, const int timeout_usec);

int32_t net_get_port(int sock);

#ifdef __cplusplus
}
#endif