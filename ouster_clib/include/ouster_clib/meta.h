#pragma once
#include "ouster_clib/types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define OUSTER_MAX_ROWS 128

typedef struct
{
	uint32_t mask;
	int offset;
	int depth;
} ouster_extract_t;

typedef struct
{
	int udp_port_lidar;
	int udp_port_imu;

	// This will not change when configuring azimuth window:
	int columns_per_frame;
	int columns_per_packet;

	// This is number of rows:
	int pixels_per_column;

	// This is the pixel format:
	ouster_profile_t profile;
	int channel_data_size;

	int col_size;
	
	int lidar_packet_size;

	// This is used to destagg hightmap range image from LiDAR:
	int pixel_shift_by_row[OUSTER_MAX_ROWS];

	// These are parameters to convert a hightmap range image from LiDAR to 3D points:
	double beam_altitude_angles[OUSTER_MAX_ROWS];
	double beam_azimuth_angles[OUSTER_MAX_ROWS];
	double beam_to_lidar_transform[16];
	double lidar_origin_to_beam_origin_mm;
	double lidar_to_sensor_transform[16];

	// This will change when configuring azimuth window.
	// The (mid0) is the start of azimuth window.
	// The (mid1) is the end of azimuth window.
	// The (midw) is the width of azimuth window.
	int mid0;
	int mid1;
	int midw;

	ouster_extract_t extract[OUSTER_QUANTITY_CHAN_FIELD_MAX];
} ouster_meta_t;

void ouster_meta_parse(char const *jsonstr, ouster_meta_t *out_meta);

void ouster_meta_dump(ouster_meta_t *meta, FILE * f);

#ifdef __cplusplus
}
#endif