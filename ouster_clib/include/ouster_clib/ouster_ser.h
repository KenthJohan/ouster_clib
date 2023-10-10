#pragma once
#include <stdint.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>



typedef struct
{
	uint32_t port;
	uint32_t size;
	char buf[];
} ouster_udpcap_t;



void ouster_udpcap_read(ouster_udpcap_t * cap, FILE * f);
void ouster_udpcap_sendto(ouster_udpcap_t * cap, int sock, struct sockaddr_in * addr);


