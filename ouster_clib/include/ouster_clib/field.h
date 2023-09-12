#pragma once
#include <stdint.h>
#include "ouster_clib/types.h"
#include "ouster_clib/meta.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	ouster_quantity_t quantity;
	uint32_t mask;
	int offset;
	int num_valid_pixels;
	int column0;
	int depth;
	int rows;
	int cols;
	int rowsize;
	void *data;
} ouster_field_t;

void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t *meta);
void ouster_field_destagger(ouster_field_t fields[], int count, ouster_meta_t *meta);
void ouster_field_apply_mask_u32(ouster_field_t *field);
void ouster_field_zero(ouster_field_t fields[], int count);

#ifdef __cplusplus
}
#endif