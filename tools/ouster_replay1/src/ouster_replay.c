#define _DEFAULT_SOURCE

#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib/client.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/lidar_column.h>
#include <ouster_clib/lidar_header.h>
#include <ouster_clib/lut.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/ouster_assert.h>
#include <ouster_clib/ouster_fs.h>
#include <ouster_clib/ouster_log.h>
#include <ouster_clib/ouster_net.h>
#include <ouster_clib/ouster_ser.h>
#include <ouster_clib/sock.h>
#include <ouster_clib/types.h>

#include <tigr/tigr.h>
#include <tigr/tigr_mouse.h>

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef struct
{
	char const *read_filename;
	FILE *read_file;
	ouster_meta_t meta;
	int socks[SOCK_INDEX_COUNT];
} app_t;

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	fs_pwd();

	app_t app = {
	    .read_filename = argv[2]};

	ouster_log("Opening file '%s'\n", app.read_filename);
	app.read_file = fopen(app.read_filename, "r");
	ouster_assert_notnull(app.read_file);

	{
		char *content = fs_readfile(argv[1]);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &app.meta);
		free(content);
	}

	printf("Column window: %i %i\n", app.meta.mid0, app.meta.mid1);

	app.socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(app.meta.udp_port_lidar);
	app.socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(app.meta.udp_port_imu);

	ouster_udpcap_t * cap = calloc(1, sizeof(ouster_udpcap_t) + NET_UDP_MAX_SIZE);
	while (1) {
		size_t rc;
		rc = fread(cap, sizeof(ouster_udpcap_t), 1, app.read_file);
		cap->size = le32toh(cap->size);
		cap->port = le32toh(cap->port);
		rc = fread(cap->buf, cap->size, 1, app.read_file);
		
		
	}

	return 0;
}
