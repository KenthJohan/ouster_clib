#pragma once


#ifdef __cplusplus
extern "C" {
#endif

void fs_pwd();
char * fs_readfile(char const * path);

#ifdef __cplusplus
}
#endif