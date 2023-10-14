// Comment out this line when using as DLL
#define ouster_clib_STATIC
#ifndef OUSTER_CLIB_H
#define OUSTER_CLIB_H

#include <stdint.h>

#define ouster_id(T) OUSTER_ID##T##ID_

typedef float ouster_f32_t;
typedef double ouster_f64_t;

// Packet Header [256 bits]

// Identifies lidar data vs. other packets in stream. Packet Type is 0x1 for Lidar packets.
typedef uint16_t ouster_packet_type_t;

// Index of the lidar scan, increments every time the sensor completes a rotation, crossing the zero azimuth angle.
typedef uint16_t ouster_frame_id_t;

// Initialization ID. Updates on every reinit, which may be triggered by the user or an error, and every reboot. 
// This value may also be obtained by running the HTTP command GET /api/v1
typedef uint32_t ouster_init_id_t;

// Serial number of the sensor. This value is unique to each sensor and can be found on a sticker affixed to the top of the sensor. 
// In addition, this information is also available on the Sensor Web UI and by reading the field prod_sn from get_sensor_info.
typedef uint64_t ouster_prod_sn_t;

// Indicates the shot limiting status of the sensor. Different codes indicates whether the sensor is in Normal Operation or in Shot Limiting. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_shot_limiting_t;

// Indicates whether thermal shutdown is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_thermal_shutdown_t;

// Countdown from 30s to indicate when shot limiting is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_countdown_shot_limiting_t;

// Countdown from 30s to indicate that thermal shutdown is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_countdown_thermal_shutdown_t;

// Column header

/// @brief Timestamp of the measurement in nanoseconds.
typedef uint64_t ouster_timestamp_t;

/// @brief Sequentially incrementing measurement counting up from 0 to 511, or 0 to 1023, or 0 to 2047 depending on lidar_mode.
typedef uint16_t ouster_measurment_id_t;

/// @brief Indicates validity of the measurements. Status is 0x01 for valid measurements. Status is 0x00 for dropped or disabled columns.
typedef uint32_t ouster_status_t;

typedef uint32_t ouster_field_range_t;
typedef uint16_t ouster_field_intensity_t;
typedef uint8_t ouster_field_reflectivity_t;
typedef uint16_t ouster_field_nearir_t; // Ambient

typedef enum
{
	ouster_id(ouster_f32_t),
	ouster_id(ouster_f64_t),
	ouster_id(ouster_measurment_id_t),
	ouster_id(ouster_timestamp_t),
	ouster_id(ouster_status_t),
	ouster_id(ouster_frame_id_t),
	ouster_id(ouster_packet_type_t),
	ouster_id(ouster_init_id_t),
	ouster_id(ouster_prod_sn_t),
	ouster_id(ouster_countdown_thermal_shutdown_t),
	ouster_id(ouster_countdown_shot_limiting_t),
	ouster_id(ouster_thermal_shutdown_t),
	ouster_id(ouster_shot_limiting_t),
	ouster_id(ouster_field_range_t),
	ouster_id(ouster_field_intensity_t),
	ouster_id(ouster_field_reflectivity_t),
	ouster_id(ouster_field_nearir_t),
	OUSTER_TYPE_LAST
} ouster_type_t;

/*
https://static.ouster.dev/sensor-docs/image_route1/image_route2/sensor_data/sensor-data.html#single-return-profile
RRRR Y0SS NN00
|--- | |- |-
4    1 2  2
0    4 6  8
*/
typedef enum
{
	OUSTER_PROFILE_LIDAR_LEGACY = 1,
	OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL = 2, // Dual Return Profile
	OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16 = 3,		// Single Return Profile
	OUSTER_PROFILE_RNG15_RFL8_NIR8 = 4,				// Low Data Rate Profile
	OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL = 5,
	OUSTER_PROFILE_COUNT
} ouster_profile_t;

typedef enum
{

	OUSTER_QUANTITY_RANGE = 1,			 // 1st return range in mm
	OUSTER_QUANTITY_RANGE2 = 2,			 // 2nd return range in mm
	OUSTER_QUANTITY_SIGNAL = 3,			 // 1st return signal in photons
	OUSTER_QUANTITY_SIGNAL2 = 4,		 // 2nd return signal in photons
	OUSTER_QUANTITY_REFLECTIVITY = 5,	 // 1st return reflectivity, calibrated by range and sensor///< sensitivity in FW 2.1+. See sensor docs for more details
	OUSTER_QUANTITY_REFLECTIVITY2 = 6,	 // 2nd return reflectivity, calibrated by range and sensor///< sensitivity in FW 2.1+. See sensor docs for more details
	OUSTER_QUANTITY_NEAR_IR = 7,		 // near_ir in photons
	OUSTER_QUANTITY_FLAGS = 8,			 // 1st return flags
	OUSTER_QUANTITY_FLAGS2 = 9,			 // 2nd return flags
	OUSTER_QUANTITY_RAW_HEADERS = 40,	 // raw headers for packet/footer/column for dev use
	OUSTER_QUANTITY_RAW32_WORD5 = 45,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD6 = 46,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD7 = 47,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD8 = 48,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD9 = 49,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_CUSTOM0 = 50,		 // custom user field
	OUSTER_QUANTITY_CUSTOM1 = 51,		 // custom user field
	OUSTER_QUANTITY_CUSTOM2 = 52,		 // custom user field
	OUSTER_QUANTITY_CUSTOM3 = 53,		 // custom user field
	OUSTER_QUANTITY_CUSTOM4 = 54,		 // custom user field
	OUSTER_QUANTITY_CUSTOM5 = 55,		 // custom user field
	OUSTER_QUANTITY_CUSTOM6 = 56,		 // custom user field
	OUSTER_QUANTITY_CUSTOM7 = 57,		 // custom user field
	OUSTER_QUANTITY_CUSTOM8 = 58,		 // custom user field
	OUSTER_QUANTITY_CUSTOM9 = 59,		 // custom user field
	OUSTER_QUANTITY_RAW32_WORD1 = 60,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD2 = 61,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD3 = 62,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD4 = 63,	 // raw word access to packet for dev use
	OUSTER_QUANTITY_CHAN_FIELD_MAX = 64, // max which allows us to introduce future fields
} ouster_quantity_t;

/*
  https://github.com/ouster-lidar/ouster_example/blob/9d0971107f6f9c95e16afd727fa2534d01a0fe4e/ouster_client/src/parsing.cpp#L155

  packet_header_size = legacy ? 0 : 32;
  col_header_size = legacy ? 16 : 12;
  channel_data_size = entry.chan_data_size;
  col_footer_size = legacy ? 4 : 0;
  packet_footer_size = legacy ? 0 : 32;

  col_size = col_header_size + pixels_per_column * channel_data_size +
			  col_footer_size;
  lidar_packet_size = packet_header_size + columns_per_packet * col_size +
					  packet_footer_size;
*/
#define OUSTER_PACKET_HEADER_SIZE 32
#define OUSTER_PACKET_FOOTER_SIZE 32
#define OUSTER_COLUMN_HEADER_SIZE 12
#define OUSTER_COLUMN_FOOTER_SIZE 0


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

typedef struct
{
	int w;
	int h;
	double *direction;
	double *offset;
} ouster_lut_t;

typedef struct
{
	ouster_timestamp_t ts;
	ouster_status_t status;
	ouster_measurment_id_t mid;
} ouster_column_t;

typedef struct
{
	ouster_frame_id_t frame_id;
	ouster_packet_type_t packet_type;
	ouster_init_id_t init_id;
	ouster_prod_sn_t prod_sn;
	ouster_countdown_thermal_shutdown_t countdown_thermal_shutdown;
	ouster_countdown_shot_limiting_t countdown_shot_limiting;
	ouster_thermal_shutdown_t thermal_shutdown;
	ouster_shot_limiting_t shot_limiting;
} ouster_lidar_header_t;

typedef struct
{
	int frame_id;
	int last_mid;
	int mid_loss;
	int num_valid_pixels;
} ouster_lidar_t;

typedef struct
{
	ouster_quantity_t quantity;
	int rows;
	int cols;
	int depth;
	int rowsize;
	int size;
	void *data;
} ouster_field_t;

typedef struct
{
	ouster_quantity_t q[OUSTER_QUANTITY_CHAN_FIELD_MAX];
} ouster_field_desc_t;


#ifndef OUSTER_ASSERT_H
#define OUSTER_ASSERT_H

#include <stdint.h>
#include <stdlib.h>

int ouster_assert_(
	const char *expr, 
	const char *file, 
	int32_t line, 
	const char *fn, 
	const char *fmt, 
	...
	);




#define ouster_assert(expr, ...) ((expr) ? (void)0: (void)(ouster_assert_(#expr, __FILE__, __LINE__, __func__, __VA_ARGS__), abort()))
#define ouster_assert_notnull(expr) ouster_assert(expr, "%s", "Should not be NULL")

#endif // OUSTER_ASSERT_H
#ifndef OUSTER_FS_H
#define OUSTER_FS_H



#ifdef __cplusplus
extern "C" {
#endif

int fs_readfile_failed_reason(char const * filename, char * buf, int len);
void fs_pwd();
char * fs_readfile(char const * path);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_FS_H
#ifndef OUSTER_NET_H
#define OUSTER_NET_H

#include <stdint.h>

// #define INET6_ADDRSTRLEN 46
#define OUSTER_NET_ADDRSTRLEN 46
#define OUSTER_NET_UDP_MAX_SIZE 65535
#define OUSTER_NET_FLAGS_NONBLOCK 0x0001
#define OUSTER_NET_FLAGS_REUSE 0x0002
#define OUSTER_NET_FLAGS_IPV6ONLY 0x0004
#define OUSTER_NET_FLAGS_IPV4 0x0008
#define OUSTER_NET_FLAGS_IPV6 0x0010
#define OUSTER_NET_FLAGS_UDP 0x0020
#define OUSTER_NET_FLAGS_TCP 0x0040
#define OUSTER_NET_FLAGS_BIND 0x0100
#define OUSTER_NET_FLAGS_CONNECT 0x0200

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int flags;
	int rcvbuf_size;
	int port;
	char const *hint_service;
	char const *hint_name;
	int rcvtimeout_sec;
	char const *group;
} ouster_net_sock_desc_t;

typedef struct
{
	char data[OUSTER_NET_ADDRSTRLEN];
} ouster_net_addr_t;

void ouster_net_addr_set_ip4(ouster_net_addr_t *addr, char const *ip);
void ouster_net_addr_set_port(ouster_net_addr_t *addr, int port);
int ouster_net_sendto(int sock, char *buf, int size, int flags, ouster_net_addr_t *addr);

int ouster_net_create(ouster_net_sock_desc_t *desc);

int64_t ouster_net_read(int sock, char *buf, int len);

uint64_t ouster_net_select(int socks[], int n, const int timeout_sec, const int timeout_usec);

int32_t ouster_net_get_port(int sock);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_NET_H
#ifndef OUSTER_META_H
#define OUSTER_META_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void ouster_meta_parse(char const *jsonstr, ouster_meta_t *out_meta);

void ouster_meta_dump(ouster_meta_t *meta, FILE *f);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_META_H
#ifndef OUSTER_LUT_H
#define OUSTER_LUT_H


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
#ifndef OUSTER_FIELD_H
#define OUSTER_FIELD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
typedef struct
{
	int rows;
	int cols;
	int column0;
	int num_valid_pixels;
} ouster_frame_t;

typedef struct
{
	ouster_quantity_t quantity;
	uint32_t mask;
	int offset;
	int depth;
	int rowsize;
} ouster_field1_t;

typedef struct
{
	void * data[3];
} ouster_field3_t;
*/




void ouster_destagger(void *data, int cols, int rows, int depth, int rowsize, int pixel_shift_by_row[]);
void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t *meta);
void ouster_field_destagger(ouster_field_t fields[], int count, ouster_meta_t *meta);
void ouster_field_apply_mask_u32(ouster_field_t *field, ouster_meta_t *meta);
void ouster_field_zero(ouster_field_t fields[], int count);
void ouster_field_cpy(ouster_field_t dst[], ouster_field_t src[], int count);


#ifdef __cplusplus
}
#endif


#endif // OUSTER_FIELD_H
#ifndef OUSTER_LIDAR_H
#define OUSTER_LIDAR_H


#ifdef __cplusplus
extern "C"
{
#endif


void ouster_lidar_header_log(ouster_lidar_header_t *p);

void ouster_lidar_header_get(char const *buf, ouster_lidar_header_t *dst);

void ouster_column_get1(char const *colbuf, void *dst, int type);

void ouster_column_get(char const *colbuf, ouster_column_t *dst);

void ouster_column_log(ouster_column_t const *column);

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount);

#ifdef __cplusplus
}
#endif


#endif // OUSTER_LIDAR_H
/**
 * @defgroup c_addons_monitor Monitor
 * @brief The monitor addon periodically tracks statistics for the world and systems.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef OUSTER_UDPCAP_H
#define OUSTER_UDPCAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>


typedef struct
{
	uint32_t port;
	uint32_t size;
	char buf[];
} ouster_udpcap_t;


void ouster_udpcap_read(ouster_udpcap_t *cap, FILE *f);

int ouster_udpcap_sendto(ouster_udpcap_t *cap, int sock, ouster_net_addr_t *addr);

void ouster_udpcap_sock_to_file(ouster_udpcap_t *cap, int sock, FILE *f);

void ouster_udpcap_set_port(ouster_udpcap_t *cap, int port);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_UDPCAP_H

/** @} */
#ifndef OUSTER_LOG_H
#define OUSTER_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void ouster_log(char const *format, ...);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_LOG_H
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

int ouster_sock_create_udp_lidar(int port);

int ouster_sock_create_udp_imu(int port);

int ouster_sock_create_tcp(char const *hint_name);

#ifdef __cplusplus
}
#endif
#ifndef OUSTER_CLIENT_H
#define OUSTER_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	char *data;
	int size;
	int cap;
} ouster_buffer_t;

typedef struct
{
	void *curl;
	char const *host;
	ouster_buffer_t buf;
} ouster_client_t;

void ouster_client_init(ouster_client_t *client);
void ouster_client_fini(ouster_client_t *client);
void ouster_client_download_meta_file(ouster_client_t *client, char const *path);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_CLIENT_H


#endif // OUSTER_CLIB_H