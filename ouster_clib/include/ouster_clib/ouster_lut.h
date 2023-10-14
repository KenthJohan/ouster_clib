#ifndef OUSTER_LUT_H
#define OUSTER_LUT_H

#include "ouster_clib.h"

#ifdef __cplusplus
extern "C" {
#endif



void ouster_lut_init(ouster_lut_t *lut, ouster_meta_t const *meta);
void ouster_lut_fini(ouster_lut_t *lut);

/** Converts 2D hightmap to pointcloud
 *
 * @param lut Input LUT unit vector direction field
 * @param range Input Raw LiDAR Sensor RANGE field 2D hightmap
 * @param xyz Output Image pointcloud
 */
void ouster_lut_cartesian_f64(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride);
void ouster_lut_cartesian_f32(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride);

double *ouster_lut_alloc(ouster_lut_t const *lut);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_LUT_H