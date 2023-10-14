#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib.h>

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	ouster_fs_pwd();

	if (argc <= 2) {
		printf("Hello welcome to %s!\n", argv[0]);
		printf("This tool downloads meta file from Ouster sensor.\n");
		printf("Examples:\n");
		printf("\t$ %s <%s> <%s>\n", argv[0], "ipaddr", "outputfile");
		printf("\t$ %s %s %s\n", argv[0], "192.168.1.137", "meta.json");
		printf("To find ip addresses on the network you can run 'arp -a'\n");
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
