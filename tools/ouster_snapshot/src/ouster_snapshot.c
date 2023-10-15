#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib.h>

#include "image_saver.h"

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef enum {
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

typedef enum {
	SNAPSHOT_MODE_UNKNOWN,
	SNAPSHOT_MODE_RAW,
	SNAPSHOT_MODE_DESTAGGER,
	MONITOR_MODE_COUNT
} monitor_mode_t;

mode_t get_mode(char const *str)
{
	if (strcmp(str, "raw") == 0) {
		return SNAPSHOT_MODE_RAW;
	}
	if (strcmp(str, "destagger") == 0) {
		return SNAPSHOT_MODE_DESTAGGER;
	}
	return SNAPSHOT_MODE_UNKNOWN;
}

void print_help(int argc, char *argv[])
{
	printf("Hello welcome to %s!\n", argv[0]);
	printf("This tool takes snapshots from LiDAR sensor and saves it as PNG image.\n");
	printf("To use this tool you will have to provide the meta file that correspond to the LiDAR sensor configuration\n");
	printf("\t$ %s <%s> <%s> <%s>\n", argv[0], "meta.json", "mode", "output_filename");
	printf("\t$ %s <%s> %s %s\n", argv[0], "meta.json", "raw", "raw.png");
	printf("\t$ %s <%s> %s %s\n", argv[0], "meta.json", "destagger", "destaggered.png");
}

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	ouster_fs_pwd();

	if (argc <= 4) {
		print_help(argc, argv);
		return 0;
	}

	monitor_mode_t mode = get_mode(argv[2]);
	if (mode == SNAPSHOT_MODE_UNKNOWN) {
		printf("The mode <%s> does not exist.\n", argv[2]);
		print_help(argc, argv);
		return 0;
	}

	ouster_meta_t meta = {0};

	{
		char *content = ouster_fs_readfile(argv[1]);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &meta);
		free(content);
		printf("Column window: %i %i\n", meta.mid0, meta.mid1);
	}

	int socks[2];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(7502);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(7503);

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, &meta);

	image_saver_t saver = {
	    .filename = argv[3],
	    .w = meta.midw,
	    .h = meta.pixels_per_column,
	    .depth = meta.extract[OUSTER_QUANTITY_RANGE].depth};
	image_saver_init(&saver);

	ouster_lidar_t lidar = {0};

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = ouster_net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char buf[OUSTER_NET_UDP_MAX_SIZE];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			//ouster_log("%-10s %5ji, mid = %04ji\n", "SOCK_LIDAR", (intmax_t)n, (intmax_t)lidar.last_mid);
			if(n == meta.lidar_packet_size)
			{
				ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
				if (lidar.last_mid == meta.mid1) {
					if (mode == SNAPSHOT_MODE_RAW) {}
					if (mode == SNAPSHOT_MODE_DESTAGGER) {
						ouster_field_destagger(fields, FIELD_COUNT, &meta);
					}
					image_saver_save(&saver, fields[FIELD_RANGE].data);
					ouster_field_zero(fields, FIELD_COUNT);
					printf("save_png frame=%i\n", lidar.frame_id);
				}
			}
			else
			{
				printf("Bytes received (%ji) does not match lidar_packet_size (%ji)\n", (intmax_t)n, (intmax_t)meta.lidar_packet_size);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			//char buf[OUSTER_NET_UDP_MAX_SIZE];
			//int64_t n = ouster_net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			//ouster_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
		}
	}

	return 0;
}
