#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "argparse.h"
#include <ouster_clib.h>

static const char *const usages[] = {
    "ouster_meta [options] [[--] args]",
    "ouster_meta [options]",
    NULL,
};

/*
if (argc <= 2) {
    printf("Hello welcome to %s!\n", argv[0]);
    printf("This tool downloads meta file from Ouster sensor.\n");
    printf("Examples:\n");
    printf("\t$ %s <%s> <%s>\n", argv[0], "ipaddr", "outputfile");
    printf("\t$ %s %s %s\n", argv[0], "192.168.1.137", "meta.json");
    printf("To find ip addresses on the network you can run 'arp -a'\n");
    return 0;
}
*/

int main(int argc, char const *argv[])
{
	printf("ouster_meta=================================================\n");
	ouster_fs_pwd();

	char const *outputfile = NULL;
	char const *host = NULL;

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('s', "sourceip", &host, "Sensor IP to download from", NULL, 0, 0),
	    OPT_STRING('m', "metafile", &outputfile, "The meta file to download", NULL, 0, 0),
	    OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, argv);

	if (outputfile == NULL) {
		argparse_usage(&argparse);
		fprintf(stderr, "Missing outputfile argument\n");
		return -1;
	}

	if (host == NULL) {
		argparse_usage(&argparse);
		fprintf(stderr, "Missing host argument\n");
		return -1;
	}

	ouster_log("Downloading meta to %s\n", outputfile);
	{
		FILE *f = fopen(outputfile, "w+");
		int s = ouster_sock_create_tcp(host, 80);
		ouster_vec_t v = {0};
		ouster_vec_init(&v, 1, 1024);
		ouster_http_request(s, host, OUSTER_HTTP_GET_METADATA, &v);
		fwrite(v.data, v.count * v.esize, 1, f);
		fclose(f);
	}

	return 0;
}
