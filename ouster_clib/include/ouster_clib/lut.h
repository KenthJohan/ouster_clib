#pragma once

#include "ouster_clib/meta.h"

typedef struct
{
	int w;
	int h;
	double *direction;
	double *offset;
} ouster_lut_t;

void ouster_lut_init(ouster_lut_t *lut, ouster_meta_t const *meta);
void ouster_lut_fini(ouster_lut_t *lut);

/** Converts 2D hightmap to pointcloud
 *
 * @param lut Input LUT unit vector direction field
 * @param range Input Raw LiDAR Sensor RANGE field 2D hightmap
 * @param xyz Output Image pointcloud
 */
void ouster_lut_cartesian_f64(ouster_lut_t const *lut, uint32_t const *range, double *out_xyz, int out_xyz_stride);
void ouster_lut_cartesian_f32(ouster_lut_t const *lut, uint32_t const *range, float *out_xyz, int out_xyz_stride);


double * ouster_lut_alloc(ouster_lut_t const *lut);