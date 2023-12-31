#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib.h>

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef enum {
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

int main(int argc, char *argv[])
{
	ouster_os_set_api_defaults();
	ouster_fs_pwd();

	if (argc <= 1) {
		printf("Missing input meta file\n");
		return 0;
	}
	/*
	ouster_client_t client =
	{
	    .host = "192.168.1.137"
	};
	ouster_client_init(&client);
	ouster_client_download_meta_file(&client, "../meta1.json");
	ouster_client_fini(&client);
	*/

	ouster_meta_t meta = {0};
	ouster_lut_t lut = {0};

	{
		char *content = ouster_fs_readfile(argv[1]);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &meta);
		free(content);
		ouster_lut_init(&lut, &meta);
		printf("Column window: %i %i\n", meta.mid0, meta.mid1);
	}

	int socks[SOCK_INDEX_COUNT] = {
	    [SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(meta.udp_port_lidar, OUSTER_DEFAULT_RCVBUF_SIZE),
	    [SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(meta.udp_port_imu, OUSTER_DEFAULT_RCVBUF_SIZE),
	};

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, &meta);

	ouster_lidar_t lidar = {0};

	double *xyz = calloc(1, lut.w * lut.h * sizeof(double) * 3);

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = ouster_net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char buf[1024 * 100];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			printf("%-10s %5ji, mid = %5ji\n", "SOCK_LIDAR", (intmax_t)n, (intmax_t)lidar.last_mid);
			ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
			if (lidar.last_mid == meta.mid1) {
				ouster_lut_cartesian_f64(&lut, fields[FIELD_RANGE].data, xyz, 3);
				ouster_field_zero(fields, FIELD_COUNT);
				printf("mid_loss %i\n", lidar.mid_loss);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			char buf[1024 * 256];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			printf("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
		}
	}

	free(xyz);

	return 0;
}
