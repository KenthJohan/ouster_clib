#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
https://aishack.in/tutorials/2d-matrices-cvmat-opencv/
*/
typedef struct
{
  int dim[4];
  int step[4];
  char * data;
  int num_valid_pixels;
} ouster_mat4_t;


void ouster_mat4_init(ouster_mat4_t * mat);
void ouster_mat4_zero(ouster_mat4_t * mat);
void ouster_mat4_apply_mask_u32(ouster_mat4_t * mat, uint32_t mask);

#ifdef __cplusplus
}
#endif