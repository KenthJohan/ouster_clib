/**
 * @file ouster_clib.h
 * @brief Ouster public API.
 *
 * This file contains the public API for ouster_clib.
 */

#ifndef OUSTER_CLIB_H
#define OUSTER_CLIB_H

/**
 * @defgroup c C API
 *
 * @{
 * @}
 */

/**
 * @defgroup core Core
 * @brief Core Ouster API functionality
 *
 * \ingroup c
 * @{
 */

/**
 * @defgroup options API defines
 * @brief Defines for customizing compile time features.
 * @{
 */

/** \def OUSTER_DEBUG
 * Used for input parameter checking and cheap sanity checks. There are lots of
 * asserts in every part of the code, so this will slow down applications.
 */
#define OUSTER_DEBUG

/** \def OUSTER_USE_CURL
 * Used to download meta file from Ouster Sensor HTTP server
 */
#define OUSTER_USE_CURL

/** \def OUSTER_USE_DUMP
 * Include dump or print struct functionality
 */
#define OUSTER_USE_DUMP

/** \def OUSTER_USE_UDPCAP
 * Include UDP capture and replay functionality
 */
#define OUSTER_USE_UDPCAP

/** \def OUSTER_ENABLE_LOG
 * Enable logging
 */
#define OUSTER_ENABLE_LOG

/** @} */ // end of options

#include <stdint.h>

/** Translate C type to id. */
#define ouster_id(T) OUSTER_ID##T##ID_

typedef float ouster_f32_t;
typedef double ouster_f64_t;

/** Packet header.
 * Identifies lidar data vs. other packets in stream.
 * Packet Type is 0x1 for Lidar packets. */
typedef uint16_t ouster_packet_type_t;

/** Index of the lidar scan, increments every time the sensor completes a rotation, crossing the zero azimuth angle. */
typedef uint16_t ouster_frame_id_t;

/** Packet header Initialization ID.
 * Updates on every reinit, which may be triggered by the user or an error, and every reboot.
 * This value may also be obtained by running the HTTP command GET /api/v1 */
typedef uint32_t ouster_init_id_t;

/** Packet header. Serial number of the sensor.
 * This value is unique to each sensor and can be found on a sticker affixed to the top of the sensor.
 * In addition, this information is also available on the Sensor Web UI and by reading the field prod_sn from get_sensor_info.
 */
typedef uint64_t ouster_prod_sn_t;

/** Packet header.
 * Indicates the shot limiting status of the sensor.
 * Different codes indicates whether the sensor is in Normal Operation or in Shot Limiting.
 * Please refer to Shot Limiting section for more details.
 */
typedef uint8_t ouster_shot_limiting_t;

/** Packet header.
 * Indicates whether thermal shutdown is imminent.
 * Please refer to Shot Limiting section for more details.
 */
typedef uint8_t ouster_thermal_shutdown_t;

/** Packet header.
 * Countdown from 30s to indicate when shot limiting is imminent.
 * Please refer to Shot Limiting section for more details.
 */
typedef uint8_t ouster_countdown_shot_limiting_t;

/** Packet header.
 * Countdown from 30s to indicate that thermal shutdown is imminent.
 * Please refer to Shot Limiting section for more details.
 */
typedef uint8_t ouster_countdown_thermal_shutdown_t;

/** Column header
 * Timestamp of the measurement in nanoseconds. */
typedef uint64_t ouster_timestamp_t;

/** Column header
 * Sequentially incrementing measurement counting up from 0 to 511, or 0 to 1023, or 0 to 2047 depending on lidar_mode. */
typedef uint16_t ouster_measurment_id_t;

/** Column header
 * Indicates validity of the measurements.
 * Status is 0x01 for valid measurements.
 * Status is 0x00 for dropped or disabled columns. */
typedef uint32_t ouster_status_t;

/** Range */
typedef uint32_t ouster_field_range_t;

/** Intensity */
typedef uint16_t ouster_field_intensity_t;

/** Reflectivity */
typedef uint8_t ouster_field_reflectivity_t;

/** Ambient */
typedef uint16_t ouster_field_nearir_t;

typedef enum {
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

typedef enum {

	OUSTER_QUANTITY_RANGE = 1,           // 1st return range in mm
	OUSTER_QUANTITY_RANGE2 = 2,          // 2nd return range in mm
	OUSTER_QUANTITY_SIGNAL = 3,          // 1st return signal in photons
	OUSTER_QUANTITY_SIGNAL2 = 4,         // 2nd return signal in photons
	OUSTER_QUANTITY_REFLECTIVITY = 5,    // 1st return reflectivity, calibrated by range and sensor///< sensitivity in FW 2.1+. See sensor docs for more details
	OUSTER_QUANTITY_REFLECTIVITY2 = 6,   // 2nd return reflectivity, calibrated by range and sensor///< sensitivity in FW 2.1+. See sensor docs for more details
	OUSTER_QUANTITY_NEAR_IR = 7,         // near_ir in photons
	OUSTER_QUANTITY_FLAGS = 8,           // 1st return flags
	OUSTER_QUANTITY_FLAGS2 = 9,          // 2nd return flags
	OUSTER_QUANTITY_RAW_HEADERS = 40,    // raw headers for packet/footer/column for dev use
	OUSTER_QUANTITY_RAW32_WORD5 = 45,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD6 = 46,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD7 = 47,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD8 = 48,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD9 = 49,    // raw word access to packet for dev use
	OUSTER_QUANTITY_CUSTOM0 = 50,        // custom user field
	OUSTER_QUANTITY_CUSTOM1 = 51,        // custom user field
	OUSTER_QUANTITY_CUSTOM2 = 52,        // custom user field
	OUSTER_QUANTITY_CUSTOM3 = 53,        // custom user field
	OUSTER_QUANTITY_CUSTOM4 = 54,        // custom user field
	OUSTER_QUANTITY_CUSTOM5 = 55,        // custom user field
	OUSTER_QUANTITY_CUSTOM6 = 56,        // custom user field
	OUSTER_QUANTITY_CUSTOM7 = 57,        // custom user field
	OUSTER_QUANTITY_CUSTOM8 = 58,        // custom user field
	OUSTER_QUANTITY_CUSTOM9 = 59,        // custom user field
	OUSTER_QUANTITY_RAW32_WORD1 = 60,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD2 = 61,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD3 = 62,    // raw word access to packet for dev use
	OUSTER_QUANTITY_RAW32_WORD4 = 63,    // raw word access to packet for dev use
	OUSTER_QUANTITY_CHAN_FIELD_MAX = 64, // max which allows us to introduce future fields
} ouster_quantity_t;

/*
 Packet Header [256 bits]
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

/** The first bytes of the UDP LIDAR packet */
#define OUSTER_PACKET_HEADER_SIZE 32

/** The last bytes of the UDP LIDAR packet */
#define OUSTER_PACKET_FOOTER_SIZE 32

/** Bytes of a column header. Multiple of this column can appear in a packet */
#define OUSTER_COLUMN_HEADER_SIZE 12

/** Column does not have any footer */
#define OUSTER_COLUMN_FOOTER_SIZE 0

/** The max number of LIDAR rows of Ouster Sensors */
#define OUSTER_MAX_ROWS 128

/** Size for SOL_SOCKET, SO_RCVBUF */
#define OUSTER_DEFAULT_RCVBUF_SIZE (1024*1024)


typedef struct
{
	uint32_t mask;
	int offset;
	int depth;
} ouster_extract_t;

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

/** @} */ // end of core

#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_field.h"
#include "ouster_clib/ouster_fs.h"
#include "ouster_clib/ouster_lidar.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_lut.h"
#include "ouster_clib/ouster_meta.h"
#include "ouster_clib/ouster_net.h"
#include "ouster_clib/ouster_sock.h"

#ifdef OUSTER_NO_UDPCAP
#undef OUSTER_USE_UDPCAP
#endif

#ifdef OUSTER_NO_DUMP
#undef OUSTER_USE_DUMP
#endif

#ifdef OUSTER_NO_CURL
#undef OUSTER_USE_CURL
#endif


#ifdef OUSTER_USE_UDPCAP
#include "ouster_clib/ouster_udpcap.h"
#endif

#ifdef OUSTER_USE_DUMP
#include "ouster_clib/ouster_dump.h"
#endif

#ifdef OUSTER_USE_CURL
#include "ouster_clib/ouster_client.h"
#endif

#endif // OUSTER_CLIB_H