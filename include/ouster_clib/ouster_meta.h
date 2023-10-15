#ifndef OUSTER_META_H
#define OUSTER_META_H

#include "ouster_clib.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void ouster_meta_parse(char const *jsonstr, ouster_meta_t *out_meta);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_META_H