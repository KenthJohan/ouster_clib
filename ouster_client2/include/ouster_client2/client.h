/*
https://static.ouster.dev/sensor-docs/image_route1/image_route2/sensor_data/sensor-data.html#lidar-data-packet-format

*/

#pragma once



int ouster_client_create_lidar_udp_socket(char const * hint_service);

int ouster_client_create_imu_udp_socket(char const * hint_service);

int ouster_client_create_imu_tcp_socket(char const * hint_name);
