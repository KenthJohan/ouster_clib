#pragma once
#include <stdint.h>


#define ouster_id(T) OUSTER_ID##T##ID_

typedef float ouster_f32_t;
typedef double ouster_f64_t;

// Timestamp of the measurement in nanoseconds.
typedef uint64_t ouster_timestamp_t;

/// @brief Sequentially incrementing measurement counting up from 0 to 511, or 0 to 1023, or 0 to 2047 depending on lidar_mode.
typedef uint16_t ouster_measurment_id_t;

/// @brief Indicates validity of the measurements. Status is 0x01 for valid measurements. Status is 0x00 for dropped or disabled columns.
typedef uint32_t ouster_status_t;

//Index of the lidar scan, increments every time the sensor completes a rotation, crossing the zero azimuth angle.
typedef uint16_t ouster_frame_id_t;

//Identifies lidar data vs. other packets in stream. Packet Type is 0x1 for Lidar packets.
typedef uint16_t ouster_packet_type_t;

//Initialization ID. Updates on every reinit, which may be triggered by the user or an error, and every reboot. This value may also be obtained by running the HTTP command GET /api/v1
typedef uint32_t ouster_init_id_t;

//Serial number of the sensor. This value is unique to each sensor and can be found on a sticker affixed to the top of the sensor. In addition, this information is also available on the Sensor Web UI and by reading the field prod_sn from get_sensor_info.
typedef uint64_t ouster_prod_sn_t;

//Countdown from 30s to indicate that thermal shutdown is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_countdown_thermal_shutdown_t;

//Countdown from 30s to indicate when shot limiting is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_countdown_shot_limiting_t;

// Indicates whether thermal shutdown is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_thermal_shutdown_t;

//Indicates the shot limiting status of the sensor. Different codes indicates whether the sensor is in Normal Operation or in Shot Limiting. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_shot_limiting_t;

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
OUSTER_TYPE_LAST
} ouster_type_t;



#define OUSTER_PROFILE_LIDAR_LEGACY 1
#define OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL 2 // Dual Return Profile
#define OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16 3 // Single Return Profile
#define OUSTER_PROFILE_RNG15_RFL8_NIR8 3 // Low Data Rate Profile
#define OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL 5

#define OUSTER_OS1_16 0
#define OUSTER_OS1_32 1
#define OUSTER_OS1_64 2
#define OUSTER_OS1_128 3


















