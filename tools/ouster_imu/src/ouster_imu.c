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
	ouster_meta_t meta = {0};
	int socks[SOCK_INDEX_COUNT];
	ouster_lidar_t lidar;

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('m', "metafile", &metafile, "The meta file that correspond to the LiDAR sensor configuration", NULL, 0, 0),
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

	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(meta.udp_port_lidar, OUSTER_DEFAULT_RCVBUF_SIZE);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(meta.udp_port_imu, OUSTER_DEFAULT_RCVBUF_SIZE);

	float ang[3] = {0};
	float acc[3] = {0};

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
			if(n == meta.lidar_packet_size)
			{
				ouster_lidar_get_fields(&lidar, &meta, buf, NULL, 0);
				if (lidar.last_mid == meta.mid1) {
					//printf("mid_loss=%ji\n", (intmax_t)lidar.mid_loss);
				}
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			char buf[OUSTER_NET_UDP_MAX_SIZE];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			if(n == OUSTER_PACKET_IMU_SIZE)
			{
				// Sanity check, should be a static assert:
				ouster_assert(sizeof(ouster_imu_packet_t) == OUSTER_PACKET_IMU_SIZE, "");

				// Cast buffer to IMU packet, this is bad ignores endianness:
				ouster_imu_packet_t *imu = (ouster_imu_packet_t *)buf;

				if (0) {
					printf("time %ju %ju %ju\n", (uintmax_t)imu->sys_ts, (uintmax_t)imu->acc_ts, (uintmax_t)imu->gyro_ts);
					printf("acc  %f %f %f\n", imu->acc[0], imu->acc[1], imu->acc[2]);
					printf("angv %f %f %f\n", imu->angvel[0], imu->angvel[1], imu->angvel[2]);
				}

				if (0) {
					// FIR coefficient:
					float coefficient = 0.5f;
					ang[0] = (coefficient * ang[0]) + ((1.0f - coefficient) * imu->angvel[0]);
					ang[1] = (coefficient * ang[1]) + ((1.0f - coefficient) * imu->angvel[1]);
					ang[2] = (coefficient * ang[2]) + ((1.0f - coefficient) * imu->angvel[2]);
					printf("angv %f %f %f\n", ang[0], ang[1], ang[2]);
				}

				if (1) {
					// FIR coefficient:
					float coefficient = 0.5f;
					acc[0] = (coefficient * acc[0]) + ((1.0f - coefficient) * imu->acc[0]);
					acc[1] = (coefficient * acc[1]) + ((1.0f - coefficient) * imu->acc[1]);
					acc[2] = (coefficient * acc[2]) + ((1.0f - coefficient) * imu->acc[2]);
					printf("acc %f %f %f\n", acc[0], acc[1], acc[2]);
				}
			}

		}
	}

	return 0;
}
