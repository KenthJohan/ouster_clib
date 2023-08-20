#pragma once
#include <stdint.h>


#define NET_FLAGS_NONBLOCK 0x01
#define NET_FLAGS_REUSE 0x02
#define NET_FLAGS_IPV6ONLY 0x04
#define NET_FLAGS_IPV4 0x08
#define NET_FLAGS_IPV6 0x10
#define NET_FLAGS_UDP 0x20
#define NET_FLAGS_BIND 0x40



typedef struct
{
    int flags;
    int rcvbuf_size;
    char const * hint_service;
    char const * hint_name;
} net_sock_desc_t;




int net_create(net_sock_desc_t * desc);


int64_t net_read(int sock, char * buf, int len);

uint64_t net_select(int socks[], int n, const int timeout_sec);