#include "ouster_clib/lut.h"
#include "ouster_clib/meta.h"
#include <math.h>
#include <stdlib.h>


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



void ouster_lut1(ouster_meta_t * meta)
{
    int w = 0;
    int h = 0;
    double * encoder = calloc(1, w * h * sizeof(double));   // theta_e
    double * azimuth = calloc(1, w * h * sizeof(double));   // theta_a
    double * altitude = calloc(1, w * h * sizeof(double));  // phi
    double azimuth_radians = M_PI * 2.0 / w;

    // populate angles for each pixel
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
}