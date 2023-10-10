

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#include <ouster_clib/ouster_ser.h>
#include <ouster_clib/ouster_assert.h>
#include <ouster_clib/ouster_fs.h>
#include <ouster_clib/ouster_log.h>
#include <ouster_clib/ouster_net.h>
#include <ouster_clib/meta.h>




typedef struct
{
	char const *metafile;
	char const *read_filename;
	char const *ip_dst;
	FILE *read_file;
	ouster_meta_t meta;
} app_t;


void print_help(int argc, char *argv[])
{
	printf("Hello welcome to %s!\n", "ouster_replay1");
	printf("This tool replays custom UDP capture file.\n");
	printf("Arguments:\n");
	printf("\targ1: The meta file that correspond to the LiDAR sensor configuration\n");
	printf("\targ2: The capture file to replay\n");
	printf("\targ3: The destination ip address. (Optional, default=127.0.0.1)\n");
	printf("Examples:\n");
	printf("\t$ %s <%s> <%s> <%s>\n", argv[0], "arg1", "arg2", "arg3");
	printf("\t$ %s <%s> <%s> <%s>\n", argv[0], "matafile", "capturefile", "ip");
	printf("\t$ %s %s %s %s\n", argv[0], "meta.json", "capture.udpcap", "127.0.0.1");
	printf("\t$ %s %s %s\n", argv[0], "meta.json", "capture.udpcap");
}


int main(int argc, char *argv[])
{
	printf("==================ouster_replay1================================\n");
	fs_pwd();

	app_t app = {0};
		
	if(argc == 3)
	{
		app.ip_dst = "127.0.0.1";
	}
	else if(argc == 4)
	{
		app.metafile = argv[1];
		app.read_filename = argv[2];
		app.ip_dst = argv[3];
		printf("Command: %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
	}
	else
	{
		print_help(argc, argv);
	}




	ouster_log("Opening file '%s'\n", app.read_filename);
	app.read_file = fopen(app.read_filename, "r");
	ouster_assert_notnull(app.read_file);

	{
		char *content = fs_readfile(app.metafile);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &app.meta);
		free(content);
	}

	printf("Column window: %i %i\n", app.meta.mid0, app.meta.mid1);

	net_sock_desc_t desc = {
		.flags = NET_FLAGS_UDP
	};
	int sock = net_create(&desc);

	struct sockaddr_in server = {0};
	server.sin_family      = AF_INET;
	server.sin_addr.s_addr = inet_addr(app.ip_dst);
	
	uint32_t packet_id = 0;
	ouster_udpcap_t * cap = calloc(1, sizeof(ouster_udpcap_t) + NET_UDP_MAX_SIZE);
	while (1) {

		ouster_udpcap_read(cap, app.read_file);
		int rc = ouster_udpcap_sendto(cap, sock, &server);
		packet_id++;
		if(rc == (int)cap->size)
		{
		}
		printf("%ji : ip=%s:%ji, size=%ji\n", (uintmax_t)packet_id, app.ip_dst, (uintmax_t)cap->port, (uintmax_t)cap->size);
		//getchar();
		nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
	}

	return 0;
}
