#include "ouster_clib/lut.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/*


XYZLut make_xyz_lut(size_t w, size_t h, double range_unit,
					const mat4d& beam_to_lidar_transform,
					const mat4d& transform,
					const std::vector<double>& azimuth_angles_deg,
					const std::vector<double>& altitude_angles_deg) {
	if (w <= 0 || h <= 0)
		throw std::invalid_argument("lut dimensions must be greater than zero");

	if ((azimuth_angles_deg.size() != h || altitude_angles_deg.size() != h) &&
		(azimuth_angles_deg.size() != w * h ||
		 altitude_angles_deg.size() != w * h)) {
		throw std::invalid_argument("unexpected scan dimensions");
	}

	double beam_to_lidar_euclidean_distance_mm = beam_to_lidar_transform(0, 3);
	if (beam_to_lidar_transform(2, 3) != 0) {
		beam_to_lidar_euclidean_distance_mm =
			std::sqrt(std::pow(beam_to_lidar_transform(0, 3), 2) +
					  std::pow(beam_to_lidar_transform(2, 3), 2));
	}

	XYZLut lut;

	Eigen::ArrayXd encoder(w * h);   // theta_e
	Eigen::ArrayXd azimuth(w * h);   // theta_a
	Eigen::ArrayXd altitude(w * h);  // phi

	if (azimuth_angles_deg.size() == h && altitude_angles_deg.size() == h) {
		// OS sensor
		const double azimuth_radians = M_PI * 2.0 / w;

		// populate angles for each pixel
		for (size_t v = 0; v < w; v++) {
			for (size_t u = 0; u < h; u++) {
				size_t i = u * w + v;
				encoder(i) = 2.0 * M_PI - (v * azimuth_radians);
				azimuth(i) = -azimuth_angles_deg[u] * M_PI / 180.0;
				altitude(i) = altitude_angles_deg[u] * M_PI / 180.0;
			}
		}

	} else if (azimuth_angles_deg.size() == w * h &&
			   altitude_angles_deg.size() == w * h) {
		// DF sensor
		// populate angles for each pixel
		for (size_t v = 0; v < w; v++) {
			for (size_t u = 0; u < h; u++) {
				size_t i = u * w + v;
				encoder(i) = 0;
				azimuth(i) = azimuth_angles_deg[i] * M_PI / 180.0;
				altitude(i) = altitude_angles_deg[i] * M_PI / 180.0;
			}
		}
	}

	// unit vectors for each pixel
	lut.direction = LidarScan::Points{w * h, 3};
	lut.direction.col(0) = (encoder + azimuth).cos() * altitude.cos();
	lut.direction.col(1) = (encoder + azimuth).sin() * altitude.cos();
	lut.direction.col(2) = altitude.sin();

	// offsets due to beam origin
	lut.offset = LidarScan::Points{w * h, 3};
	lut.offset.col(0) =
		encoder.cos() * beam_to_lidar_transform(0, 3) -
		lut.direction.col(0) * beam_to_lidar_euclidean_distance_mm;
	lut.offset.col(1) =
		encoder.sin() * beam_to_lidar_transform(0, 3) -
		lut.direction.col(1) * beam_to_lidar_euclidean_distance_mm;
	lut.offset.col(2) =
		-lut.direction.col(2) * beam_to_lidar_euclidean_distance_mm +
		beam_to_lidar_transform(2, 3);

	// apply the supplied transform
	auto rot = transform.topLeftCorner(3, 3).transpose();
	auto trans = transform.topRightCorner(3, 1).transpose();
	lut.direction.matrix() *= rot;
	lut.offset.matrix() *= rot;
	lut.offset.matrix() += trans.replicate(w * h, 1);

	// apply scaling factor
	lut.direction *= range_unit;
	lut.offset *= range_unit;

	return lut;
}
*/

#define FMTF "%+20.10f"
#define FMTF3 FMTF FMTF FMTF "\n"
#define FMTF4 FMTF FMTF FMTF FMTF "\n"

#define M4_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], (x)[15]
#define M4_ARGS_2(x) (x)[0], (x)[4], (x)[8], (x)[12], (x)[1], (x)[5], (x)[9], (x)[13], (x)[2], (x)[6], (x)[10], (x)[14], (x)[3], (x)[7], (x)[11], (x)[15]
#define M4_FORMAT FMTF4 FMTF4 FMTF4 FMTF4

#define M3_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8]
#define M3_ARGS_2(x) (x)[0], (x)[3], (x)[6], (x)[1], (x)[4], (x)[7], (x)[2], (x)[5], (x)[8]
#define M3_FORMAT FMTF3 FMTF3 FMTF3

#define V3_ARGS(x) (x)[0], (x)[1], (x)[2]
#define V3_FORMAT "%f %f %f\n"
#define V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

void print_m4(double const *a)
{
	printf(M4_FORMAT, M4_ARGS_1(a));
}

void print_m3(double const *a)
{
	printf(M3_FORMAT, M3_ARGS_1(a));
}

void print_v3(double const *a)
{
	printf(V3_FORMAT, V3_ARGS(a));
}

#define M4(i, j) ((i) * 4 + (j))
#define M3(i, j) ((i) * 3 + (j))

void mul3(double *r, double const *a, double const *x)
{
	double temp[3];
	temp[0] = (a[M3(0, 0)] * x[0]) + (a[M3(1, 0)] * x[1]) + (a[M3(2, 0)] * x[2]);
	temp[1] = (a[M3(0, 1)] * x[0]) + (a[M3(1, 1)] * x[1]) + (a[M3(2, 1)] * x[2]);
	temp[2] = (a[M3(0, 2)] * x[0]) + (a[M3(1, 2)] * x[1]) + (a[M3(2, 2)] * x[2]);
	r[0] = temp[0];
	r[1] = temp[1];
	r[2] = temp[2];
}

void ouster_lut_fini(ouster_lut_t *lut)
{
	free(lut->direction);
	free(lut->offset);
	lut->direction = NULL;
	lut->offset = NULL;
}

/*
https://static.ouster.dev/sensor-docs/image_route1/image_route3/sensor_data/sensor-data.html?highlight=cartesian#lidar-range-to-xyz

*/
void ouster_lut_init(ouster_lut_t *lut, ouster_meta_t const *meta)
{
	assert(lut);
	assert(meta);

	int w = meta->mid1 - meta->mid0 + 1;
	int h = meta->pixels_per_column;
	assert(w >= 0);
	assert(w <= 1024);
	assert(h >= 0);
	assert(h <= 128);

	double *encoder = calloc(1, w * h * sizeof(double));  // theta_e
	double *azimuth = calloc(1, w * h * sizeof(double));  // theta_a
	double *altitude = calloc(1, w * h * sizeof(double)); // phi
	double *direction = calloc(1, w * h * 3 * sizeof(double));
	double *offset = calloc(1, w * h * 3 * sizeof(double));
	assert(encoder);
	assert(azimuth);
	assert(altitude);
	assert(direction);
	assert(offset);

	float beam_to_lidar_transform_03 = meta->beam_to_lidar_transform[M4(0, 3)];
	float beam_to_lidar_transform_23 = meta->beam_to_lidar_transform[M4(2, 3)];
	print_m4(meta->lidar_to_sensor_transform);

	// This represent a column measurement angle:
	double azimuth_radians = M_PI * 2.0 / meta->columns_per_frame;

	// OS sensor - populate angles for each pixel
	for (int c = 0; c < w; c++)
	{
		for (int r = 0; r < h; r++)
		{
			// Row major - each row is continuous memory
			int mid = meta->mid0 + c;
			assert(mid >= meta->mid0);
			assert(mid <= meta->mid1);
			int i = r * w + mid;
			encoder[i] = 2.0 * M_PI - (mid * azimuth_radians);
			azimuth[i] = -meta->beam_azimuth_angles[r] * M_PI / 180.0;
			altitude[i] = meta->beam_altitude_angles[r] * M_PI / 180.0;
		}
	}

	// unit vectors for each pixel
	for (int i = 0; i < w * h; i++)
	{
		direction[i * 3 + 0] = cos(encoder[i] + azimuth[i]) * cos(altitude[i]);
		direction[i * 3 + 1] = sin(encoder[i] + azimuth[i]) * cos(altitude[i]);
		direction[i * 3 + 2] = sin(altitude[i]);
	}

	for (int i = 0; i < w * h; i++)
	{
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

	for (int i = 0; i < w * h; ++i)
	{
		double *d = direction + i * 3;
		double *o = offset + i * 3;
		mul3(d, rotation, d);
		mul3(o, rotation, o);
		o[0] += translation[0];
		o[1] += translation[1];
		o[2] += translation[2];
	}

	free(encoder);
	free(azimuth);
	free(altitude);

	lut->direction = direction;
	lut->offset = offset;
	lut->w = w;
	lut->h = h;

	for (int i = 0; i < w * h; ++i)
	{
		// double *d = direction + i * 3;
		// double *o = offset + i * 3;
		//  printf("%+f %+f %+f %+f %+f %+f, %+f\n", o[0], o[1], o[2], d[0], d[1], d[2], sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]));
	}
}

void ouster_lut_cartesian(ouster_lut_t const *lut, uint32_t const *range, double *out_xyz)
{
	double const *d = lut->direction;
	double const *o = lut->offset;
	for (int i = 0; i < (lut->w * lut->h); ++i, out_xyz += 3, d += 3, o += 3)
	{
		double mag = range[i];
		out_xyz[0] = mag * d[0] + o[0];
		out_xyz[1] = mag * d[1] + o[1];
		out_xyz[2] = mag * d[2] + o[2];
		// printf("%+f %+f\n", mag, sqrt(V3_DOT(out_xyz, out_xyz)));
	}
}
