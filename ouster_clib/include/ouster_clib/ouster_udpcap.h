#pragma once
#include <stdint.h>
#include <stdio.h>

#include "ouster_clib/ouster_net.h"

typedef struct
{
	uint32_t port;
	uint32_t size;
	char buf[];
} ouster_udpcap_t;

void ouster_udpcap_read(ouster_udpcap_t *cap, FILE *f);

int ouster_udpcap_sendto(ouster_udpcap_t *cap, int sock, net_addr_t *addr);

void ouster_udpcap_sock_to_file(ouster_udpcap_t *cap, int sock, FILE *f);

void ouster_udpcap_set_port(ouster_udpcap_t *cap, int port);
