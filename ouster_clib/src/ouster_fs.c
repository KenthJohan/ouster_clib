#include "ouster_clib/ouster_fs.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_log.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_FILENAME "\033[38;2;255;150;50m" // Orange
#define COLOR_CWD "\033[38;2;255;150;50m" // Orange
#define COLOR_ERROR "\033[38;2;255;50;50m" // Red
#define COLOR_RST "\033[0m"

int fs_readfile_failed_reason(char const * filename, char * buf, int len)
{
	char const * e = strerror(errno);
	char cwd[1024] = {0};
	char *rc = getcwd(cwd, sizeof(cwd));
	int n = snprintf(buf, len, "cwd:"COLOR_CWD"%s " COLOR_RST "path:"COLOR_FILENAME"%s " COLOR_ERROR"%s"COLOR_RST, rc?rc:"", filename, e);
	return n;
}

void fs_pwd()
{
	char cwd[1024] = {0};
	char *rc = getcwd(cwd, sizeof(cwd));
	if (rc == NULL) {
		ouster_log("getcwd error: %s\n", strerror(errno));
		return;
	}
	printf("Current working directory: %s%s%s\n", "\033[38;2;100;100;255m", cwd, "\033[0m");
}

char *fs_readfile(char const *path)
{
	ouster_assert_notnull(path);
	char *content = NULL;

	FILE *file = fopen(path, "r");
	if (file == NULL) {
		goto error;
	}

	fseek(file, 0, SEEK_END);
	int32_t size = (int32_t)ftell(file);

	if (size == -1) {
		goto error;
	}
	rewind(file);

	content = malloc(size + 1);
	content[size] = '\0';
	size_t n = fread(content, size, 1, file);
	if (n != 1) {
		ouster_log("%s: could not read wholef file %d bytes\n", path, size);
		goto error;
	}
	ouster_assert(content[size] == '\0', "Expected null terminator");
	fclose(file);
	return content;
error:
	if (content) {
		free(content);
	}
	return NULL;
}