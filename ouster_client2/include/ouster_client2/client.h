#pragma once


typedef struct 
{
    int socks[2];
    char * buffer;
    int buffer_count;
    int buffer_cap;
} ouster_client_t;


void ouster_client_init(ouster_client_t * client, char const * host);
