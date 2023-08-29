#pragma once
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define ouster_id(T) OUSTER_ID##T##ID_

typedef float ouster_f32_t;
typedef double ouster_f64_t;



// Packet Header [256 bits]

//Identifies lidar data vs. other packets in stream. Packet Type is 0x1 for Lidar packets.
typedef uint16_t ouster_packet_type_t;

//Index of the lidar scan, increments every time the sensor completes a rotation, crossing the zero azimuth angle.
typedef uint16_t ouster_frame_id_t;

//Initialization ID. Updates on every reinit, which may be triggered by the user or an error, and every reboot. This value may also be obtained by running the HTTP command GET /api/v1
typedef uint32_t ouster_init_id_t;

//Serial number of the sensor. This value is unique to each sensor and can be found on a sticker affixed to the top of the sensor. In addition, this information is also available on the Sensor Web UI and by reading the field prod_sn from get_sensor_info.
typedef uint64_t ouster_prod_sn_t;

//Indicates the shot limiting status of the sensor. Different codes indicates whether the sensor is in Normal Operation or in Shot Limiting. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_shot_limiting_t;

// Indicates whether thermal shutdown is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_thermal_shutdown_t;

//Countdown from 30s to indicate when shot limiting is imminent. Please refer to Shot Limiting section for more details.
typedef uint8_t ouster_countdown_shot_limiting_t;

//Countdown from 30s to indicate that thermal shutdown is imminent. Please refer to Shot Limiting section for more details.
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
  OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16 = 3, // Single Return Profile
  OUSTER_PROFILE_RNG15_RFL8_NIR8 = 4, // Low Data Rate Profile
  OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL = 5,
  OUSTER_PROFILE_COUNT
} ouster_profile_t;



typedef enum{

    OUSTER_QUANTITY_RANGE = 1,            // 1st return range in mm
    OUSTER_QUANTITY_RANGE2 = 2,           // 2nd return range in mm
    OUSTER_QUANTITY_SIGNAL = 3,           // 1st return signal in photons
    OUSTER_QUANTITY_SIGNAL2 = 4,          // 2nd return signal in photons
    OUSTER_QUANTITY_REFLECTIVITY = 5,     // 1st return reflectivity, calibrated by range and sensor///< sensitivity in FW 2.1+. See sensor docs for more details
    OUSTER_QUANTITY_REFLECTIVITY2 = 6,    // 2nd return reflectivity, calibrated by range and sensor///< sensitivity in FW 2.1+. See sensor docs for more details
    OUSTER_QUANTITY_NEAR_IR = 7,          // near_ir in photons
    OUSTER_QUANTITY_FLAGS = 8,            // 1st return flags
    OUSTER_QUANTITY_FLAGS2 = 9,           // 2nd return flags
    OUSTER_QUANTITY_RAW_HEADERS = 40,     // raw headers for packet/footer/column for dev use
    OUSTER_QUANTITY_RAW32_WORD5 = 45,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD6 = 46,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD7 = 47,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD8 = 48,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD9 = 49,     // raw word access to packet for dev use
    OUSTER_QUANTITY_CUSTOM0 = 50,         // custom user field
    OUSTER_QUANTITY_CUSTOM1 = 51,         // custom user field
    OUSTER_QUANTITY_CUSTOM2 = 52,         // custom user field
    OUSTER_QUANTITY_CUSTOM3 = 53,         // custom user field
    OUSTER_QUANTITY_CUSTOM4 = 54,         // custom user field
    OUSTER_QUANTITY_CUSTOM5 = 55,         // custom user field
    OUSTER_QUANTITY_CUSTOM6 = 56,         // custom user field
    OUSTER_QUANTITY_CUSTOM7 = 57,         // custom user field
    OUSTER_QUANTITY_CUSTOM8 = 58,         // custom user field
    OUSTER_QUANTITY_CUSTOM9 = 59,         // custom user field
    OUSTER_QUANTITY_RAW32_WORD1 = 60,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD2 = 61,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD3 = 62,     // raw word access to packet for dev use
    OUSTER_QUANTITY_RAW32_WORD4 = 63,     // raw word access to packet for dev use
    OUSTER_QUANTITY_CHAN_FIELD_MAX = 64,  // max which allows us to introduce future fields
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




#ifdef __cplusplus
}
#endif