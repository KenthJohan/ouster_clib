#define _DEFAULT_SOURCE
#include <endian.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ouster_clib/ouster_ser.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_assert.h"

/*
void ouster_ser_i32(char data[4], int32_t value)
{
	data[0] = ((uint32_t)value >> 24) & 0xFF;
	data[1] = ((uint32_t)value >> 16) & 0xFF;
	data[2] = ((uint32_t)value >> 8) & 0xFF;
	data[3] = ((uint32_t)value >> 0) & 0xFF;
}
*/


void ouster_udpcap_read(ouster_udpcap_t * cap, FILE * f)
{
	{
		// First get the header info
		size_t rc;
		rc = fread(cap, sizeof(ouster_udpcap_t), 1, f);
		ouster_assert(rc == 1, "");
		// Convert to little endian to host
		cap->size = le32toh(cap->size);
		cap->port = le32toh(cap->port);
	}

	{
		// Read the UDP content
		size_t rc;
		rc = fread(cap->buf, cap->size, 1, f);
		ouster_assert(rc == 1, "");
	}
}


int ouster_udpcap_sendto(ouster_udpcap_t * cap, int sock, void * addr)
{
	// Send UDP content to the the same port as the capture
	struct sockaddr_in * addr4 = addr;
	ssize_t rc;
	addr4->sin_port = htons(cap->port);
	rc = sendto(sock, cap->buf, cap->size, 0,(struct sockaddr *)addr, sizeof(struct sockaddr_in));
	//printf("rc %ji\n", (intmax_t)rc);
	return rc;
}

