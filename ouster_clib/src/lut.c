#include "ouster_clib/lut.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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

//[d0, d1] = R * [d0 d1]

#define M4_ARGS_1(x) (x)[0],(x)[1],(x)[2],(x)[3] , (x)[4],(x)[5],(x)[6],(x)[7] , (x)[8],(x)[9],(x)[10],(x)[11] , (x)[12],(x)[13],(x)[14],(x)[15]
#define M4_ARGS_2(x) (x)[0],(x)[4],(x)[8],(x)[12] , (x)[1],(x)[5],(x)[9],(x)[13] , (x)[2],(x)[6],(x)[10],(x)[14] , (x)[3],(x)[7],(x)[11],(x)[15]
#define M4_FORMAT "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n"
void print_m4(double * a)
{
    printf(M4_FORMAT, M4_ARGS_2(a));
}


void mul(double * r, double * a, double * b)
{
    r[0] = 
    (a[0] * b[0]) + 
    (a[0] * b[1]) + 
    (a[0] * b[2]);
    r[1] = 
    (a[1] * b[0]) + 
    (a[1] * b[1]) + 
    (a[1] * b[2]);
    r[2] = 
    (a[2] * b[0]) + 
    (a[2] * b[1]) + 
    (a[2] * b[2]);
}



void ouster_lut1(ouster_meta_t * meta)
{
    int w = 0;
    int h = 0;
    double * encoder = calloc(1, w * h * sizeof(double));   // theta_e
    double * azimuth = calloc(1, w * h * sizeof(double));   // theta_a
    double * altitude = calloc(1, w * h * sizeof(double));  // phi
    double * direction = calloc(1, w * h * 3 * sizeof(double));
    double * offset = calloc(1, w * h * 3 * sizeof(double));

    float beam_to_lidar_transform_03 = meta->beam_to_lidar_transform[0];
    float beam_to_lidar_transform_23 = meta->beam_to_lidar_transform[0];

    // OS sensor - populate angles for each pixel
    double azimuth_radians = M_PI * 2.0 / w;
    for (int v = 0; v < w; v++)
    {
        for (int u = 0; u < h; u++)
        {
            int i = u * w + v;
            encoder[i] = 2.0 * M_PI - (v * azimuth_radians);
            azimuth[i] = -meta->beam_azimuth_angles[u] * M_PI / 180.0;
            altitude[i] = meta->beam_altitude_angles[u] * M_PI / 180.0;
        }
    }

    // unit vectors for each pixel
    for (int i = 0; i < w*h; i++)
    {
        direction[i*3 + 0] = cos(encoder[i] + azimuth[i]) * cos(altitude[i]);
        direction[i*3 + 1] = sin(encoder[i] + azimuth[i]) * cos(altitude[i]);
        direction[i*3 + 2] = sin(altitude[i]);
    }

    for (int i = 0; i < w*h; i++)
    {
        offset[i*3 + 0] = cos(encoder[i]) * beam_to_lidar_transform_03 - direction[i*3 + 0] * meta->lidar_origin_to_beam_origin_mm;
        offset[i*3 + 1] = sin(encoder[i]) * beam_to_lidar_transform_03 - direction[i*3 + 1] * meta->lidar_origin_to_beam_origin_mm;
        offset[i*3 + 2] = beam_to_lidar_transform_23                   - direction[i*3 + 2] * meta->lidar_origin_to_beam_origin_mm;
    }

    print_m4(meta->lidar_to_sensor_transform);

    float trans[3] =
    {
        meta->lidar_to_sensor_transform[0*4 + 3],
        meta->lidar_to_sensor_transform[1*4 + 3],
        meta->lidar_to_sensor_transform[2*4 + 3],
    };
    for (int i = 0; i < w*h; i++)
    {
        offset[i*3 + 0] += trans[0];
        offset[i*3 + 0] += trans[1];
        offset[i*3 + 0] += trans[2];
    }

}