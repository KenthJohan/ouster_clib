#include "ouster_clib/os_file.h"
#include "ouster_clib/log.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

char * ouster_os_file_read(char const * path)
{
    char* content = NULL;

    FILE* file = fopen(path, "r");
    if (file == NULL) 
    {
        ouster_log("%s (%s)\n", strerror(errno), path);
        goto error;
    }

    fseek(file, 0 , SEEK_END);
    int32_t size = (int32_t)ftell(file);

    if (size == -1)
    {
        goto error;
    }
    rewind(file);

    content = malloc(size + 1);
    content[size] = '\0';
    size_t n = fread(content, size, 1, file);
    if(n != 1)
    {
        ouster_log("%s: could not read wholef file %d bytes\n", path, size);
        goto error;
    }
    assert(content[size] == '\0');
    fclose(file);
    return content;


error:
    if(content)
    {
        free(content);
    }
    return NULL;
}