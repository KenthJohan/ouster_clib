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
#include <ouster_clib/ouster_udpcap.h>
#include <ouster_clib/sock.h>
#include <ouster_clib/types.h>

#include <tigr/tigr.h>
#include <tigr/tigr_mouse.h>

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef enum {
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

typedef struct
{
	char const *write_filename;
	FILE *write_file;
	ouster_meta_t meta;
	ouster_lut_t lut;
	int socks[SOCK_INDEX_COUNT];
	ouster_lidar_t lidar;
	ouster_udpcap_t *cap_lidar;
	ouster_udpcap_t *cap_imu;
} app_t;

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	fs_pwd();

	app_t app = {
	    .write_filename = argv[2]};

	ouster_log("Opening file '%s'\n", app.write_filename);
	app.write_file = fopen(app.write_filename, "w");
	ouster_assert_notnull(app.write_file);

	{
		char *content = fs_readfile(argv[1]);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &app.meta);
		free(content);
	}

	ouster_lut_init(&app.lut, &app.meta);
	printf("Column window: %i %i\n", app.meta.mid0, app.meta.mid1);

	app.socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(app.meta.udp_port_lidar);
	app.socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(app.meta.udp_port_imu);

	app.cap_lidar = calloc(1, sizeof(ouster_udpcap_t) + NET_UDP_MAX_SIZE);
	app.cap_imu = calloc(1, sizeof(ouster_udpcap_t) + NET_UDP_MAX_SIZE);
	app.cap_lidar->port = htole32(app.meta.udp_port_lidar);
	app.cap_imu->port = htole32(app.meta.udp_port_imu);

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, &app.meta);

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = net_select(app.socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
			continue;
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char *buf = app.cap_lidar->buf;
			int64_t n = net_read(app.socks[SOCK_INDEX_LIDAR], buf, NET_UDP_MAX_SIZE);

			app.cap_lidar->size = n;
			int64_t write_size = sizeof(ouster_udpcap_t) + n;
			int rc = fwrite((void*)app.cap_lidar, write_size, 1, app.write_file);

			if (n == app.meta.lidar_packet_size) {
				ouster_lidar_get_fields(&app.lidar, &app.meta, buf, fields, FIELD_COUNT);
				if (app.lidar.last_mid == app.meta.mid1) {
					printf("LIDAR size=%ji, mid_loss=%ji, rc=%i\n", (intmax_t)n, (intmax_t)app.lidar.mid_loss, rc);
				}
			} else {
				printf("LIDAR size=%ji, lidar_packet_size=%ji, rc=%i\n", (intmax_t)n, (intmax_t)app.meta.lidar_packet_size, rc);
			}

		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			char *buf = app.cap_imu->buf;
			int64_t n = net_read(app.socks[SOCK_INDEX_IMU], buf, NET_UDP_MAX_SIZE);
			if (n > 0) {
				//printf("IMU size=%ji\n", (intmax_t)n);
			}
		}
	}

	return 0;
}
