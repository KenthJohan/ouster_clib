#pragma once



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
    int rcvbuf;
    char const * hint_service;
    char const * hint_name;
} net_sock_desc_t;




int net_create(net_sock_desc_t * desc);