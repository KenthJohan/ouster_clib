#include "ouster_clib.h"
#include <stddef.h>


int ouster_sock_create_udp_lidar(int port, int rcvbuf_size)
{
	ouster_assert(rcvbuf_size >= -1, "");
	ouster_net_sock_desc_t desc = {0};
	desc.flags = OUSTER_NET_FLAGS_UDP | OUSTER_NET_FLAGS_NONBLOCK | OUSTER_NET_FLAGS_REUSE | OUSTER_NET_FLAGS_BIND;
	desc.hint_name = NULL;
	desc.rcvbuf_size = (rcvbuf_size == -1) ? OUSTER_DEFAULT_RCVBUF_SIZE : rcvbuf_size;
	desc.hint_service = NULL;
	desc.port = port;
	// desc.group = "239.201.201.201";
	// desc.group = "239.255.255.250";
	return ouster_net_create(&desc);
}

int ouster_sock_create_udp_imu(int port, int rcvbuf_size)
{
	ouster_assert(rcvbuf_size >= -1, "");
	ouster_net_sock_desc_t desc = {0};
	desc.flags = OUSTER_NET_FLAGS_UDP | OUSTER_NET_FLAGS_NONBLOCK | OUSTER_NET_FLAGS_REUSE | OUSTER_NET_FLAGS_BIND;
	desc.hint_name = NULL;
	desc.rcvbuf_size = (rcvbuf_size == -1) ? OUSTER_DEFAULT_RCVBUF_SIZE : rcvbuf_size;
	desc.hint_service = NULL;
	desc.port = port;
	return ouster_net_create(&desc);
}

int ouster_sock_create_tcp(char const *hint_name)
{
	ouster_assert_notnull(hint_name);

	ouster_net_sock_desc_t desc = {0};
	desc.flags = OUSTER_NET_FLAGS_TCP | OUSTER_NET_FLAGS_CONNECT;
	desc.hint_name = hint_name;
	desc.hint_service = "7501";
	desc.rcvtimeout_sec = 10;
	return ouster_net_create(&desc);
}
