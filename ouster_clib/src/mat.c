#include "ouster_clib/mat.h"
#include <stdlib.h>
#include <string.h>


void ouster_mat4_init(ouster_mat4_t * mat)
{
    mat->step[0] =                mat->dim[0];
    mat->step[1] = mat->step[0] * mat->dim[1];
    mat->step[2] = mat->step[1] * mat->dim[2];
    mat->step[3] = mat->step[2] * mat->dim[3];
    mat->data = calloc(mat->step[3], 1);
    mat->num_valid_pixels = 0;
}


void ouster_mat4_zero(ouster_mat4_t * mat)
{
    memset(mat->data, 0, mat->step[3]);
}


void ouster_mat4_apply_mask_u32(ouster_mat4_t * mat, uint32_t mask)
{
    uint32_t * data32 = (uint32_t *)mat->data;
    for(int i = 0; i < mat->dim[1] * mat->dim[2]; ++i)
    {
        data32[i] &= mask;
    }
}