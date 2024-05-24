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

void print_help(int argc, char *argv[])
{
	printf("Hello welcome to %s!\n", "ouster_capture1");
	printf("This tool captures custom UDP capture file.\n");
	printf("Arguments:\n");
	printf("\targ1: The meta file that correspond to the LiDAR sensor configuration\n");
	printf("\targ2: The capture file to destination\n");
	printf("Examples:\n");
	printf("\t$ %s <%s> <%s>\n", argv[0], "arg1", "arg2");
	printf("\t$ %s <%s> <%s>\n", argv[0], "matafile", "capturefile");
	printf("\t$ %s %s %s\n", argv[0], "meta.json", "capture.udpcap");
}

static const char *const usages[] = {
    "ouster_capture1 [options] [[--] args]",
    "ouster_capture1 [options]",
    NULL,
};

















int main(int argc, char const *argv[])
{
	printf("ouster_capture1======================\n");
	ouster_os_set_api_defaults();
	ouster_fs_pwd();

	char const *metafile = NULL;
	char const *write_filename = NULL;
	FILE *write_file = NULL;
	ouster_udpcap_t *cap_lidar = NULL;
	ouster_udpcap_t *cap_imu = NULL;
	ouster_meta_t meta = {0};
	int socks[SOCK_INDEX_COUNT];
	ouster_lidar_t lidar;
	int is_json = 0;

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('m', "metafile", &metafile, "The meta file that correspond to the LiDAR sensor configuration", NULL, 0, 0),
	    OPT_STRING('c', "capture", &write_filename, "The capture file to write", NULL, 0, 0),
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

	if (write_filename == NULL) {
		argparse_usage(&argparse);
		return -1;
	}




	{
		ouster_assert_notnull(metafile);
		char *content = ouster_fs_readfile(metafile);
		if (content == NULL) {
			char buf[1024];
			ouster_fs_readfile_failed_reason(metafile, buf, sizeof(buf));
			printf("%s\n", buf);
			return 0;
		}
		ouster_meta_parse(content, &meta);
		free(content);
		ouster_dump_meta(stdout, &meta);
	}

	ouster_field_t fields[FIELD_COUNT] = {
	[FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};
	ouster_field_init(fields, FIELD_COUNT, &meta);

	ouster_lut_t lut = {0};
	ouster_lut_init(&lut, &meta);
	double *xyz = calloc(1, lut.w * lut.h * sizeof(double) * 3);

	ouster_assert_notnull(write_filename);
	ouster_log("Opening file '%s'\n", write_filename);
	write_file = fopen(write_filename, "w");
	if (write_file == NULL) {
		char buf[1024];
		ouster_fs_readfile_failed_reason(write_filename, buf, sizeof(buf));
		fprintf(stderr, "%s", buf);
		return -1;
	}

	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(meta.udp_port_lidar, OUSTER_DEFAULT_RCVBUF_SIZE);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(meta.udp_port_imu, OUSTER_DEFAULT_RCVBUF_SIZE);

	cap_lidar = calloc(1, sizeof(ouster_udpcap_t) + OUSTER_NET_UDP_MAX_SIZE);
	cap_imu = calloc(1, sizeof(ouster_udpcap_t) + OUSTER_NET_UDP_MAX_SIZE);
	ouster_udpcap_set_port(cap_lidar, meta.udp_port_lidar);
	ouster_udpcap_set_port(cap_imu, meta.udp_port_imu);

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = ouster_net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
			continue;
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char buf[OUSTER_NET_UDP_MAX_SIZE];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			if (n == meta.lidar_packet_size) {
				ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
				if (lidar.last_mid == meta.mid1) {
					ouster_lut_cartesian_f64(&lut, fields[FIELD_RANGE].data, xyz, sizeof(double)*3);
					ouster_field_zero(fields, FIELD_COUNT);
					printf("frame1=%i, mid_loss=%i\n", lidar.frame_id, lidar.mid_loss);
					for(int i = 0; i < (lut.w * lut.h); ++i) {
						double * p = xyz + (i*3);
						fprintf(write_file, "%f, %f, %f\n", p[0], p[1], p[3]);
					}
					fflush(write_file);
					exit(0);
				}
			} else {
				printf("Bytes received (%ji) does not match lidar_packet_size (%ji)\n", (intmax_t)n, (intmax_t)meta.lidar_packet_size);
			}
		}
		

		if (a & (1 << SOCK_INDEX_IMU)) {
			//cap_imu->size = OUSTER_NET_UDP_MAX_SIZE;
			//ouster_udpcap_sock_to_file(cap_imu, socks[SOCK_INDEX_IMU], write_file);
		}
	}

	return 0;
}
