#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

int ouster_sock_create_udp_lidar(int port);

int ouster_sock_create_udp_imu(int port);

int ouster_sock_create_tcp(char const *hint_name);

#ifdef __cplusplus
}
#endif