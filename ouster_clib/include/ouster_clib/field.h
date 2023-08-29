#pragma once
#include <stdint.h>
#include "ouster_clib/mat.h"
#include "ouster_clib/types.h"
#include "ouster_clib/meta.h"



#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    ouster_quantity_t quantity;
    uint32_t mask;
    int offset;
    ouster_mat4_t mat;
    int num_valid_pixels;
} ouster_field_t;




void ouster_field_init(ouster_field_t * field, ouster_meta_t * meta);


#ifdef __cplusplus
}
#endif