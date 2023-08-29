/*
https://static.ouster.dev/sensor-docs/image_route1/image_route2/sensor_data/sensor-data.html#lidar-data-packet-format

*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int ouster_sock_create_udp_lidar(char const * hint_service);

int ouster_sock_create_udp_imu(char const * hint_service);

int ouster_sock_create_tcp(char const * hint_name);




#ifdef __cplusplus
}
#endif