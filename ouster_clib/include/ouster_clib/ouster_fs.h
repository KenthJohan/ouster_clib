#ifndef OUSTER_FS_H
#define OUSTER_FS_H



#ifdef __cplusplus
extern "C" {
#endif

int fs_readfile_failed_reason(char const * filename, char * buf, int len);
void fs_pwd();
char * fs_readfile(char const * path);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_FS_H