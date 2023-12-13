/**
 * @defgroup sock UDP Capture
 * @brief This creates sockets
 * 
 * \ingroup ouster_sock
 * @{
 */

#ifndef OUSTER_SOCK_H
#define OUSTER_SOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

int ouster_sock_create_udp_lidar(int port, int rcvbuf_size);

int ouster_sock_create_udp_imu(int port, int rcvbuf_size);

int ouster_sock_create_tcp(char const *hint_name, int port);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_SOCK_H

/** @} */