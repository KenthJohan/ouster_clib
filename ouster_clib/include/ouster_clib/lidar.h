#pragma once
#include "ouster_clib/types.h"
#include "ouster_clib/meta.h"
#include "ouster_clib/field.h"

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct
{
	int frame_id;
	int last_mid;
	int mid_loss;
	int num_valid_pixels;
} ouster_lidar_t;

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount);

#ifdef __cplusplus
}
#endif