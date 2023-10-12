#pragma once


#ifdef __cplusplus
extern "C" {
#endif

int fs_readfile_failed_reason(char const * filename, char * buf, int len);
void fs_pwd();
char * fs_readfile(char const * path);

#ifdef __cplusplus
}
#endif