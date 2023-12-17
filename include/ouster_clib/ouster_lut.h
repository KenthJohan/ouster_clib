/**
 * @defgroup lut XYZ vector field lookup table
 * @brief Provides a vector field that converts image to pointcloud
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_LUT_H
#define OUSTER_LUT_H

#include "ouster_clib/ouster_meta.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	int w;
	int h;
	double *direction;
	double *offset;
} ouster_lut_t;

/** Inits a xyz lut table from meta configuration
 *
 * @param lut The xyz lut table
 * @param meta meta configuration
 */
void ouster_lut_init(ouster_lut_t *lut, ouster_meta_t const *meta);

/** Frees memory of xyz lut table
 *
 * @param lut The xyz lut table
 */
void ouster_lut_fini(ouster_lut_t *lut);

/** Converts 2D hightmap to pointcloud
 *
 * @param lut Input LUT unit vector direction field
 * @param range Input Raw LiDAR Sensor RANGE field 2D hightmap
 * @param xyz Output Image pointcloud
 */
void ouster_lut_cartesian_f64(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride);

/** Converts 2D hightmap to pointcloud
 *
 * @param lut Input LUT unit vector direction field
 * @param range Input Raw LiDAR Sensor RANGE field 2D hightmap
 * @param xyz Output Image pointcloud
 */
void ouster_lut_cartesian_f32(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride);

void ouster_lut_cartesian_f32_single(ouster_lut_t const *lut, float x, float y, float mag, float *out);

/** Allocates size for xyz pointcloud
 *
 * @param lut Input LUT unit vector direction field
 */
double *ouster_lut_alloc(ouster_lut_t const *lut);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_LUT_H

/** @} */