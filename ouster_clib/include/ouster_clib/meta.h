#pragma once
#include "ouster_clib/types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define OUSTER_MAX_ROWS 128

typedef struct
{

	int columns_per_frame;
	int columns_per_packet;
	int pixels_per_column;
	ouster_profile_t profile;
	int channel_data_size;
	int col_size;
	int lidar_packet_size;
	int pixel_shift_by_row[OUSTER_MAX_ROWS];

	double beam_altitude_angles[OUSTER_MAX_ROWS];
	double beam_azimuth_angles[OUSTER_MAX_ROWS];
	double beam_to_lidar_transform[16];
	double lidar_origin_to_beam_origin_mm;

	double lidar_to_sensor_transform[16];

	int mid0;
	int mid1;
	int midw;

} ouster_meta_t;

void ouster_meta_parse(char const *jsonstr, ouster_meta_t *out_meta);

#ifdef __cplusplus
}
#endif