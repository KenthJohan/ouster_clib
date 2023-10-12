#define _DEFAULT_SOURCE
#include <endian.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_net.h"
#include "ouster_clib/ouster_udpcap.h"

/*
void ouster_ser_i32(char data[4], int32_t value)
{
    data[0] = ((uint32_t)value >> 24) & 0xFF;
    data[1] = ((uint32_t)value >> 16) & 0xFF;
    data[2] = ((uint32_t)value >> 8) & 0xFF;
    data[3] = ((uint32_t)value >> 0) & 0xFF;
}
*/

void ouster_udpcap_read(ouster_udpcap_t *cap, FILE *f)
{
	ouster_assert_notnull(cap);
	ouster_assert_notnull(f);
	

	{
		uint32_t maxsize = cap->size;
		// First get the header info
		size_t rc;
		rc = fread(cap, sizeof(ouster_udpcap_t), 1, f);
		ouster_assert(rc == 1, "");
		// Convert to little endian to host
		cap->size = le32toh(cap->size);
		cap->port = le32toh(cap->port);
		ouster_assert(cap->size <= maxsize, "");
	}


	{
		// Read the UDP content
		size_t rc;
		rc = fread(cap->buf, cap->size, 1, f);
		ouster_assert(rc == 1, "");
	}
}

int ouster_udpcap_sendto(ouster_udpcap_t *cap, int sock, net_addr_t *addr)
{
	ouster_assert_notnull(cap);

	ssize_t rc;
	// Send UDP content to the the same port as the capture
	net_addr_set_port(addr, cap->port);
	rc = net_sendto(sock, cap->buf, cap->size, 0, addr);
	// printf("rc %ji\n", (intmax_t)rc);
	return rc;
}

void ouster_udpcap_set_port(ouster_udpcap_t *cap, int port)
{
	ouster_assert_notnull(cap);

	cap->port = htole32(port);
}

void ouster_udpcap_sock_to_file(ouster_udpcap_t *cap, int sock, FILE *f)
{
	ouster_assert_notnull(cap);
	ouster_assert_notnull(f);

	int64_t write_size;

	{
		int64_t n = net_read(sock, cap->buf, cap->size);
		ouster_assert(n >= 0, "");
		ouster_assert(n <= cap->size, "");
		cap->size = htole32(n);
		write_size = sizeof(ouster_udpcap_t) + n;
	}

	{
		size_t n = fwrite((void *)cap, write_size, 1, f);
		ouster_assert(n == 1, "");
	}
}