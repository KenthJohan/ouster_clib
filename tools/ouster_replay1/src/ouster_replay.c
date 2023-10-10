

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <ouster_clib/ouster_ser.h>
#include <ouster_clib/ouster_assert.h>
#include <ouster_clib/ouster_fs.h>
#include <ouster_clib/ouster_log.h>
#include <ouster_clib/ouster_net.h>
#include <ouster_clib/meta.h>



#include <netdb.h>
#include <stdio.h>

typedef struct
{
	char const *read_filename;
	FILE *read_file;
	ouster_meta_t meta;
} app_t;

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	printf("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
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

	net_sock_desc_t desc = {
		.flags = NET_FLAGS_UDP
	};
	int sock = net_create(&desc);

	struct sockaddr_in server;
	server.sin_family      = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[3]);

	ouster_udpcap_t * cap = calloc(1, sizeof(ouster_udpcap_t) + NET_UDP_MAX_SIZE);
	while (1) {

		ouster_udpcap_read(cap, app.read_file);
		ouster_udpcap_sendto(cap, sock, &server);
		getchar();
	}

	return 0;
}
