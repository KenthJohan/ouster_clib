#include <ouster_clib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE (1024 * 1024)

int main(int argc, char *argv[])
{
	ouster_os_set_api_defaults();
	ouster_fs_pwd();

	if (argc <= 1) {
		printf("Missing host address\n");
		return 0;
	}

	int s = ouster_sock_create_tcp(argv[1], 80);
	ouster_vec_t v = {0};
	ouster_vec_init(&v, 1, 1024);
	ouster_http_request(s, argv[1], OUSTER_HTTP_GET_METADATA, &v);
	printf("%s\n\n", (char *)v.data);
	return 0;

	return 0;
}
