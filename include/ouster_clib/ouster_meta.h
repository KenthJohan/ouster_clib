#ifndef OUSTER_META_H
#define OUSTER_META_H

#include "ouster_clib/ouster_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
https://static.ouster.dev/sensor-docs/image_route1/image_route2/sensor_data/sensor-data.html#single-return-profile
RRRR Y0SS NN00
|--- | |- |-
4    1 2  2
0    4 6  8
*/
typedef enum {
	OUSTER_PROFILE_LIDAR_LEGACY = 1,
	/** Dual Return Profile */
	OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL = 2,
	/** Single Return Profile */
	OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16 = 3,
	/** Low Data Rate Profile */
	OUSTER_PROFILE_RNG15_RFL8_NIR8 = 4,
	OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL = 5,
	OUSTER_PROFILE_COUNT
} ouster_profile_t;

typedef struct
{
	/** The configured port where Ouster Sensor will send LIDAR UDP packets */
	int udp_port_lidar;

	/** The configured port where Ouster Sensor will send IMU UDP packets */
	int udp_port_imu;

	/** This will not change when configuring azimuth window */
	int columns_per_frame;

	/** This will not change when configuring azimuth window */
	int columns_per_packet;

	/** This is number of rows */
	int pixels_per_column;

	/** This is the pixel format */
	ouster_profile_t profile;
	int channel_data_size;

	int col_size;

	/** UDP packet size for LIDAR packets */
	int lidar_packet_size;

	/** This is used to destagg hightmap range image from LiDAR */
	int pixel_shift_by_row[OUSTER_MAX_ROWS];

	/** These are parameters to convert a hightmap range image from LiDAR to 3D points */
	double beam_altitude_angles[OUSTER_MAX_ROWS];
	double beam_azimuth_angles[OUSTER_MAX_ROWS];
	double beam_to_lidar_transform[16];
	double lidar_origin_to_beam_origin_mm;
	double lidar_to_sensor_transform[16];

	/** Start of azimuth window.
	 * This will change when configuring azimuth window. */
	int mid0;

	/** End of azimuth window.
	 * This will change when configuring azimuth window. */
	int mid1;

	/** Width of azimuth window.
	 * This will change when configuring azimuth window. */
	int midw;

	ouster_extract_t extract[OUSTER_QUANTITY_CHAN_FIELD_MAX];
} ouster_meta_t;

void ouster_meta_parse(char const *jsonstr, ouster_meta_t *out_meta);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_META_H