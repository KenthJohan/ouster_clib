#pragma once


typedef struct 
{
    int socket_lidar;
    int socket_imu;
} ouster_client_t;


void ouster_client_init(ouster_client_t * client, char const * host);
