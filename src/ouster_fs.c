#include "ouster_clib.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define COLOR_FILENAME "\033[38;2;150;150;255m" // Blue
#define COLOR_CWD "\033[38;2;150;150;255m"      // Blue
#define COLOR_ERROR "\033[38;2;255;50;50m"      // Red
#define COLOR_RST "\033[0m"

int ouster_fs_readfile_failed_reason(char const *filename, char *buf, int len)
{
	char const *e = strerror(errno);
	char cwd[1024] = {0};
	char *rc = getcwd(cwd, sizeof(cwd));
	rc = rc ? rc : "";
	int n = snprintf(buf, len, "cwd:%s%s%s path:%s%s%s error:%s%s%s",
	                 COLOR_CWD, rc, COLOR_RST,
	                 COLOR_FILENAME, filename, COLOR_RST,
	                 COLOR_ERROR, e, COLOR_RST);
	return n;
}

void ouster_fs_pwd()
{
	char cwd[1024] = {0};
	char *rc = getcwd(cwd, sizeof(cwd));
	if (rc == NULL) {
		ouster_log("getcwd error: %s\n", strerror(errno));
		return;
	}
	printf("Current working directory: %s%s%s\n", COLOR_CWD, cwd, COLOR_RST);
}

char *ouster_fs_readfile(char const *path)
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

	content = ouster_os_malloc(size + 1);
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
		ouster_os_free(content);
	}
	return NULL;
}