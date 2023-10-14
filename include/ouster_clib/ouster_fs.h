#ifndef OUSTER_FS_H
#define OUSTER_FS_H



#ifdef __cplusplus
extern "C" {
#endif

int ouster_fs_readfile_failed_reason(char const * filename, char * buf, int len);
void ouster_fs_pwd();
char * ouster_fs_readfile(char const * path);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_FS_H