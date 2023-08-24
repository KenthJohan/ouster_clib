/*
https://static.ouster.dev/sensor-docs/image_route1/image_route2/sensor_data/sensor-data.html#lidar-data-packet-format

*/

#pragma once



typedef struct 
{
    int socks[2];
    int sock_tcp;
    char * buffer;
    int buffer_count;
    int buffer_cap;
} ouster_client_t;


void ouster_client_init(ouster_client_t * client, char const * host);
