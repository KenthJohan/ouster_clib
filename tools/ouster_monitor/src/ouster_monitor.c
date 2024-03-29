#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib.h>
#include "argparse.h"

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
	MONITOR_MODE_UNKNOWN,
	MONITOR_MODE_LOSS,
	MONITOR_MODE_PACKET,
	MONITOR_MODE_HEADER,
	MONITOR_MODE_COLUMN,
	MONITOR_MODE_COUNT
} monitor_mode_t;

mode_t get_mode(char const *str)
{
	if (strcmp(str, "loss") == 0) {
		return MONITOR_MODE_LOSS;
	}
	if (strcmp(str, "packet") == 0) {
		return MONITOR_MODE_PACKET;
	}
	if (strcmp(str, "header") == 0) {
		return MONITOR_MODE_HEADER;
	}
	if (strcmp(str, "column") == 0) {
		return MONITOR_MODE_COLUMN;
	}
	return MONITOR_MODE_UNKNOWN;
}

void print_columns(ouster_meta_t *meta, char const *buf)
{
	char const *colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
	for (int icol = 0; icol < meta->columns_per_packet; icol++, colbuf += meta->col_size) {
		ouster_column_t column = {0};
		ouster_column_get(colbuf, &column);
		ouster_dump_column(stdout, &column);
	}
}

void print_help(int argc, char *argv[])
{
	printf("Hello welcome to %s!\n", argv[0]);
	printf("This tool monitors UDP packages.\n");
	printf("To use this tool you will have to provide the meta file that correspond to the LiDAR sensor configuration\n");
	printf("\t$ %s <%s> %s\n", argv[0], "meta.json", "loss");
	printf("\t$ %s <%s> %s\n", argv[0], "meta.json", "packet");
	printf("\t$ %s <%s> %s\n", argv[0], "meta.json", "header");
	printf("\t$ %s <%s> %s\n", argv[0], "meta.json", "column");
}

static const char *const usages[] = {
    "ouster_monitor [options] [[--] args]",
    "ouster_monitor [options]",
    NULL,
};


int main(int argc, char const *argv[])
{
	printf("ouster_monitor======================================================\n");
	ouster_os_set_api_defaults();
	ouster_fs_pwd();

	char const *metafile = NULL;
	char const *modestr = NULL;

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('m', "metafile", &metafile, "The meta file that correspond to the LiDAR sensor configuration", NULL, 0, 0),
	    OPT_STRING('e', "emode", &modestr, "The printing output mode. Allowed modes: loss, packet, header, column", NULL, 0, 0),
	    OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, argv);

	if (metafile == NULL) {
		argparse_usage(&argparse);
		return -1;
	}

	monitor_mode_t mode = get_mode(modestr);

	if (mode == MONITOR_MODE_UNKNOWN) {
		printf("The mode <%s> does not exist.\n", modestr);
		argparse_usage(&argparse);
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
		char *content = ouster_fs_readfile(metafile);
		if (content == NULL) {
			char buf[512];
			ouster_fs_readfile_failed_reason(metafile, buf, sizeof(buf));
			fprintf(stderr, "%s", buf);
			return -1;
		}
		ouster_meta_parse(content, &meta);
		free(content);
		ouster_lut_init(&lut, &meta);
		printf("Column window: %i %i\n", meta.mid0, meta.mid1);
	}

	int socks[2];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(7502, OUSTER_DEFAULT_RCVBUF_SIZE);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(7503, OUSTER_DEFAULT_RCVBUF_SIZE);
	// int sock_tcp = ouster_sock_create_tcp("192.168.1.137");

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, &meta);
	// ouster_field_init(fields + 1, &meta);

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
			if (mode == MONITOR_MODE_PACKET) {
				printf("%-10s %5ji, mid = %04ji\n", "SOCK_LIDAR", (intmax_t)n, (intmax_t)lidar.last_mid);
			}
			ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
			if (mode == MONITOR_MODE_HEADER) {
				ouster_lidar_header_t header = {0};
				ouster_lidar_header_get(buf, &header);
				ouster_dump_lidar_header(stdout, &header);
			}

			if (mode == MONITOR_MODE_COLUMN) {
				print_columns(&meta, buf);
			}

			if (lidar.last_mid == meta.mid1) {
				ouster_lut_cartesian_f64(&lut, fields[FIELD_RANGE].data, xyz, 3);
				// printf("mat = %i of %i\n", fields[0].num_valid_pixels, fields[0].mat.dim[1] * fields[0].mat.dim[2]);
				ouster_field_zero(fields, FIELD_COUNT);
				if (mode == MONITOR_MODE_LOSS) {
					ouster_lidar_header_t header = {0};
					ouster_lidar_header_get(buf, &header);
					printf("mid_loss=%i, frame=%i\n", lidar.mid_loss, header.frame_id);
				}
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			char buf[1024 * 256];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			if (mode == MONITOR_MODE_PACKET) {
				ouster_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
			}
		}
	}

	return 0;
}
