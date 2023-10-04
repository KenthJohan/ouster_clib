#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



#include <ouster_clib/sock.h>
#include <ouster_clib/client.h>
#include <ouster_clib/types.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lut.h>
#include <ouster_clib/lidar_header.h>
#include <ouster_clib/lidar_column.h>
#include <ouster_clib/ouster_fs.h>
#include <ouster_clib/ouster_net.h>
#include <ouster_clib/ouster_log.h>

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	fs_pwd();

	if (argc <= 2)
	{
		printf("Hello welcome to %s!\n", argv[0]);
		printf("This tool downloads meta file from Ouster sensor.\n");
		printf("\t$ %s <%s> <%s>\n", argv[0], "ipaddr", "outputfile");
		printf("\t$ %s %s %s\n", argv[0], "192.168.1.137", "meta.json");
		return 0;
	}

	char const *host = argv[1];
	char const *outputfile = argv[2];

	ouster_client_t client =
		{
			.host = host};
	ouster_client_init(&client);
	ouster_client_download_meta_file(&client, outputfile);
	ouster_client_fini(&client);

	return 0;
}
