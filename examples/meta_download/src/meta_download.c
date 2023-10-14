#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ouster_clib.h>

int main(int argc, char *argv[])
{
	ouster_fs_pwd();

	if (argc <= 1)
	{
		printf("Missing host address\n");
		return 0;
	}

	if (argc <= 2)
	{
		printf("Missing output meta file destination\n");
		return 0;
	}

	ouster_client_t client =
		{
			.host = argv[1]};
	ouster_client_init(&client);
	ouster_client_download_meta_file(&client, argv[2]);
	ouster_client_fini(&client);
	return 0;
}
