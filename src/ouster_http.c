#include "ouster_clib.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HTTP_EOL "\r\n\r\n"

void ouster_http_request(int sock, char const *host, char const *path, ouster_vec_t *v)
{
	ouster_assert_notnull(host);
	ouster_assert_notnull(path);
	ouster_assert_notnull(v);
	ouster_assert(v->esize == 1, "");
	{
		char buf[1024] = {0};
		int n = snprintf(buf, sizeof(buf), "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: */*" HTTP_EOL, path, host);
		ouster_log("REQ START:\n%s\nREQ END\n", buf);
		ssize_t written = write(sock, buf, n);
		if(n != written) {
			ouster_log("write failed: %s\n", buf);
			return;
		}
	}

	while (1) {
		char buf[1024] = {0};
		ssize_t n = recv(sock, buf, sizeof(buf), 0);
		if (n <= 0) {
			break;
		}
		ouster_vec_append(v, buf, n, 1.5f);
	}
	{
		char *e = OUSTER_OFFSET(v->data, v->esize * v->count);
		e[0] = '\0';
	}

	char *json = strstr(v->data, HTTP_EOL);
	if (json) {
		json += strlen(HTTP_EOL);
		size_t n = strlen(json);
		memcpy(v->data, json, n);
		v->count = n;
		char *e = OUSTER_OFFSET(v->data, v->esize * v->count);
		e[0] = '\0';
	}
}