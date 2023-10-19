

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <ouster_clib.h>

#include "argparse.h"

typedef struct
{
	char const *metafile;
	char const *read_filename;
	char const *ip_dst;
	FILE *read_file;
	ouster_meta_t meta;
	int offset;
} app_t;

static const char *const usages[] = {
    "ouster_replay1 [options] [[--] args]",
    "ouster_replay1 [options]",
    NULL,
};

int main(int argc, char const *argv[])
{
	printf("ouster_replay1================================\n");
	ouster_fs_pwd();

	app_t app = {
	    .ip_dst = "127.0.0.1",
	    .offset = 0};

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('m', "metafile", &app.metafile, "The meta file that correspond to the LiDAR sensor configuration", NULL, 0, 0),
	    OPT_STRING('c', "capture", &app.read_filename, "The capture file to replay", NULL, 0, 0),
	    OPT_STRING('d', "destination", &app.ip_dst, "The destination ip address. (Optional, default=127.0.0.1)", NULL, 0, 0),
	    OPT_INTEGER('o', "offset", &app.offset, "Where to start replay", NULL, 0, 0),
	    OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, argv);

	if (app.metafile == NULL) {
		argparse_usage(&argparse);
		return -1;
	}

	if (app.ip_dst == NULL) {
		argparse_usage(&argparse);
		return -1;
	}

	if (app.read_filename == NULL) {
		argparse_usage(&argparse);
		return -1;
	}

	ouster_log("Opening file '%s'\n", app.read_filename);

	app.read_file = fopen(app.read_filename, "r");
	if (app.read_file == NULL) {
		char buf[1024];
		ouster_fs_readfile_failed_reason(app.read_filename, buf, 1024);
		fprintf(stderr, "%s", buf);
		return -1;
	}

	{
		char *content = ouster_fs_readfile(app.metafile);
		if (content == NULL) {
			char buf[1024];
			ouster_fs_readfile_failed_reason(app.read_filename, buf, 1024);
			fprintf(stderr, "%s", buf);
			return -1;
		}
		ouster_meta_parse(content, &app.meta);
		free(content);
	}

	printf("Column window: %i %i\n", app.meta.mid0, app.meta.mid1);

	ouster_net_sock_desc_t desc = {
	    .flags = OUSTER_NET_FLAGS_UDP};
	int sock = ouster_net_create(&desc);

	ouster_net_addr_t dst = {0};
	ouster_net_addr_set_ip4(&dst, app.ip_dst);

	uint32_t packet_id = 0;
	ouster_udpcap_t *cap = calloc(1, sizeof(ouster_udpcap_t) + OUSTER_NET_UDP_MAX_SIZE);

	for (int i = 0; i < app.offset; ++i) {
		cap->size = OUSTER_NET_UDP_MAX_SIZE;
		int nread = ouster_udpcap_read(cap, app.read_file);
		if (nread != OUSTER_UDPCAP_OK) {
			fprintf(stderr, "error: ouster_udpcap_read: %i\n", nread);
			return -1;
		}
		packet_id++;
	}

	while (1) {
		cap->size = OUSTER_NET_UDP_MAX_SIZE;

		int nread = ouster_udpcap_read(cap, app.read_file);
		if (nread != OUSTER_UDPCAP_OK) {
			fprintf(stderr, "error: ouster_udpcap_read: %i\n", nread);
			return -1;
		}
		int nsend = ouster_udpcap_sendto(cap, sock, &dst);
		if (nsend != (int)cap->size) {

			fprintf(stderr, "error: ouster_udpcap_sendto: %i\n", nsend);
			return -1;
		}

		packet_id++;
		printf("%ju : ip=%s:%ji, sent=%ji of %ji\n", (uintmax_t)packet_id, app.ip_dst, (intmax_t)cap->port, (intmax_t)nsend, (intmax_t)cap->size);
		// getchar();
		nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
	}

	return 0;
}
