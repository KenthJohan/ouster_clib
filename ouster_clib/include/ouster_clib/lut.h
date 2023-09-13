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
 * This operation runs a single system manually. It is an efficient way to
 * invoke logic on a set of entities, as manual systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * @param lut Input LUT unit vector direction field
 * @param range Input Raw LiDAR Sensor RANGE field 2D hightmap
 * @param xyz Output Image pointcloud
 */
void ouster_lut_cartesian(ouster_lut_t const *lut, uint32_t const *range, double *xyz);
