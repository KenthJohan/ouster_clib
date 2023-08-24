#include "ouster_client2/client.h"
#include "ouster_client2/log.h"
#include "ouster_client2/net.h"
#include "ouster_client2/lidar_header.h"
#include "ouster_client2/lidar_column.h"


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>







int ouster_client_create_lidar_udp_socket(char const * hint_service)
{
    net_sock_desc_t desc = {0};
    desc.flags = NET_FLAGS_UDP | NET_FLAGS_NONBLOCK | NET_FLAGS_REUSE | NET_FLAGS_BIND;
    desc.hint_name = NULL;
    desc.rcvbuf_size = 256 * 1024;
    desc.hint_service = hint_service;
    return net_create(&desc);
}

int ouster_client_create_imu_udp_socket(char const * hint_service)
{
    net_sock_desc_t desc = {0};
    desc.flags = NET_FLAGS_UDP | NET_FLAGS_NONBLOCK | NET_FLAGS_REUSE | NET_FLAGS_BIND;
    desc.hint_name = NULL;
    desc.rcvbuf_size = 256 * 1024;
    desc.hint_service = hint_service;
    return net_create(&desc);
}

int ouster_client_create_imu_tcp_socket(char const * hint_name)
{
    net_sock_desc_t desc = {0};
    desc.flags = NET_FLAGS_TCP | NET_FLAGS_CONNECT;
    desc.hint_name = hint_name;
    desc.hint_service = "7501";
    desc.rcvtimeout_sec = 10;
    return net_create(&desc);
}


