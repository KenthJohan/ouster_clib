#include "ouster_clib/ouster_lut.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_os_api.h"
#include "ouster_math.h"

//#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>


void ouster_lut_fini(ouster_lut_t *lut)
{
	ouster_os_free(lut->direction);
	ouster_os_free(lut->offset);
	lut->direction = NULL;
	lut->offset = NULL;
}

/*
https://static.ouster.dev/sensor-docs/image_route1/image_route3/sensor_data/sensor-data.html?highlight=cartesian#lidar-range-to-xyz

*/
void ouster_lut_init(ouster_lut_t *lut, ouster_meta_t const *meta)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(meta);

	int w = meta->midw;
	int h = meta->pixels_per_column;
	ouster_assert(w >= 0, "");
	ouster_assert(w <= 1024, "");
	ouster_assert(h >= 0, "");
	ouster_assert(h <= 128, "");

	double *encoder = ouster_os_calloc(w * h * sizeof(double));  // theta_e
	double *azimuth = ouster_os_calloc(w * h * sizeof(double));  // theta_a
	double *altitude = ouster_os_calloc(w * h * sizeof(double)); // phi
	double *direction = ouster_os_calloc(w * h * 3 * sizeof(double));
	double *offset = ouster_os_calloc(w * h * 3 * sizeof(double));
	ouster_assert_notnull(encoder);
	ouster_assert_notnull(azimuth);
	ouster_assert_notnull(altitude);
	ouster_assert_notnull(direction);
	ouster_assert_notnull(offset);

	float beam_to_lidar_transform_03 = meta->beam_to_lidar_transform[M4(0, 3)];
	float beam_to_lidar_transform_23 = meta->beam_to_lidar_transform[M4(2, 3)];
	m4_print(meta->lidar_to_sensor_transform);

	// This represent a column measurement angle:
	double azimuth_radians = OUSTER_M_PI * 2.0 / meta->columns_per_frame;

	// OS sensor - populate angles for each pixel
	for (int c = 0; c < w; c++) {
		for (int r = 0; r < h; r++) {
			// Row major - each row is continuous memory
			int mid = meta->mid0 + c;
			ouster_assert(mid >= meta->mid0, "");
			ouster_assert(mid <= meta->mid1, "");
			int i = r * w + c;
			encoder[i] = 2.0 * OUSTER_M_PI - (mid * azimuth_radians);
			azimuth[i] = -meta->beam_azimuth_angles[r] * OUSTER_M_PI / 180.0;
			altitude[i] = meta->beam_altitude_angles[r] * OUSTER_M_PI / 180.0;
		}
	}

	// unit vectors for each pixel
	for (int i = 0; i < w * h; i++) {
		direction[i * 3 + 0] = cos(encoder[i] + azimuth[i]) * cos(altitude[i]);
		direction[i * 3 + 1] = sin(encoder[i] + azimuth[i]) * cos(altitude[i]);
		direction[i * 3 + 2] = sin(altitude[i]);
	}

	for (int i = 0; i < w * h; i++) {
		offset[i * 3 + 0] = cos(encoder[i]) * beam_to_lidar_transform_03 - direction[i * 3 + 0] * meta->lidar_origin_to_beam_origin_mm;
		offset[i * 3 + 1] = sin(encoder[i]) * beam_to_lidar_transform_03 - direction[i * 3 + 1] * meta->lidar_origin_to_beam_origin_mm;
		offset[i * 3 + 2] = beam_to_lidar_transform_23 - direction[i * 3 + 2] * meta->lidar_origin_to_beam_origin_mm;
	}

	// Extract the rotation matrix from transform
	double rotation[9] = {
	    [M3(0, 0)] = meta->lidar_to_sensor_transform[M4(0, 0)],
	    [M3(1, 0)] = meta->lidar_to_sensor_transform[M4(0, 1)],
	    [M3(2, 0)] = meta->lidar_to_sensor_transform[M4(0, 2)],
	    [M3(0, 1)] = meta->lidar_to_sensor_transform[M4(1, 0)],
	    [M3(1, 1)] = meta->lidar_to_sensor_transform[M4(1, 1)],
	    [M3(2, 1)] = meta->lidar_to_sensor_transform[M4(1, 2)],
	    [M3(0, 2)] = meta->lidar_to_sensor_transform[M4(2, 0)],
	    [M3(1, 2)] = meta->lidar_to_sensor_transform[M4(2, 1)],
	    [M3(2, 2)] = meta->lidar_to_sensor_transform[M4(2, 2)],
	};

	// Extract the translation vector from transform
	float translation[3] =
	    {
	        meta->lidar_to_sensor_transform[0 * 4 + 3],
	        meta->lidar_to_sensor_transform[1 * 4 + 3],
	        meta->lidar_to_sensor_transform[2 * 4 + 3],
	    };

	for (int i = 0; i < w * h; ++i) {
		double *d = direction + i * 3;
		double *o = offset + i * 3;
		mul3(d, rotation, d);
		mul3(o, rotation, o);
		o[0] += translation[0];
		o[1] += translation[1];
		o[2] += translation[2];
	}

	ouster_os_free(encoder);
	ouster_os_free(azimuth);
	ouster_os_free(altitude);

	for (int i = 0; i < w * h; ++i) {
		double range_unit = 0.001f;
		//double range_unit = 1.0f;
		double *d = direction + i * 3;
		double *o = offset + i * 3;
		d[0] *= range_unit;
		d[1] *= range_unit;
		d[2] *= range_unit;
		o[0] *= range_unit;
		o[1] *= range_unit;
		o[2] *= range_unit;
	}

	lut->direction = direction;
	lut->offset = offset;
	lut->w = w;
	lut->h = h;

	/*
	for (int i = 0; i < w * h; ++i) {
	    double *d = direction + i * 3;
	    double *o = offset + i * 3;
	    printf("%+f %+f %+f %+f %+f %+f, %+f\n", o[0], o[1], o[2], d[0], d[1], d[2], sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]));
	}
	*/
}

void ouster_lut_cartesian_f64(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(range);
	ouster_assert_notnull(out);

	double const *d = lut->direction;
	double const *o = lut->offset;
	char *out8 = out;

	for (int i = 0; i < (lut->w * lut->h); ++i, out8 += out_stride, d += 3, o += 3) {
		double mag = range[i];
		double *outd = (double *)out8;
		outd[0] = (float)(mag * d[0] + o[0]);
		outd[1] = (float)(mag * d[1] + o[1]);
		outd[2] = (float)(mag * d[2] + o[2]);
		// printf("%+f %+f\n", mag, sqrt(V3_DOT(out_xyz, out_xyz)));
	}
}

void ouster_lut_cartesian_f32_single(ouster_lut_t const *lut, float x, float y, float mag, float *out)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(out);
	int i = round(y) * lut->w + round(x);
	ouster_assert(i >= 0, "");
	ouster_assert(i < lut->w * lut->h, "");
	double const *d = lut->direction + i;
	double const *o = lut->offset + i;
	out[0] = (float)(mag * d[0] + o[0]);
	out[1] = (float)(mag * d[1] + o[1]);
	out[2] = (float)(mag * d[2] + o[2]);
}

void ouster_lut_cartesian_f32(ouster_lut_t const *lut, uint32_t const *range, void *out, int out_stride)
{
	ouster_assert_notnull(lut);
	ouster_assert_notnull(range);
	ouster_assert_notnull(out);

	double const *d = lut->direction;
	double const *o = lut->offset;
	char *out8 = out;

	for (int i = 0; i < (lut->w * lut->h); ++i, out8 += out_stride, d += 3, o += 3) {
		double mag = range[i];
		float *outf = (float *)out8;
		outf[0] = (float)(mag * d[0] + o[0]);
		outf[1] = (float)(mag * d[1] + o[1]);
		outf[2] = (float)(mag * d[2] + o[2]);
		// printf("%+f %+f\n", mag, sqrt(V3_DOT(out_xyz, out_xyz)));
	}
}

double *ouster_lut_alloc(ouster_lut_t const *lut)
{
	ouster_assert_notnull(lut);

	int n = lut->w * lut->h;
	int size = n * sizeof(double) * 3;
	void *memory = ouster_os_calloc(size);
	return memory;
}