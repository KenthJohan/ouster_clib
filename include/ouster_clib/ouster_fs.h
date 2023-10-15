/**
 * @defgroup fs Files
 * @brief Read files
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_FS_H
#define OUSTER_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert fs_readfile error to string
 *
 * @param filename filename
 * @param buf out error buffer string
 * @param len error buffer string length
 * @return int the length that got written to buffer string
 */
int ouster_fs_readfile_failed_reason(char const *filename, char *buf, int len);

/** Prints current working directory */
void ouster_fs_pwd();

/** Read whole file and allocates memory
 *
 * @param path filename to read
 */
char *ouster_fs_readfile(char const *path);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_FS_H

/** @} */