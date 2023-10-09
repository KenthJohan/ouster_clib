#pragma once
#include <stdint.h>



typedef struct
{
	uint32_t port;
	uint32_t size;
	char buf[];
} ouster_udpcap_t;




