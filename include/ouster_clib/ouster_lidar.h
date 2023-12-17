#ifndef OUSTER_LIDAR_H
#define OUSTER_LIDAR_H

#include "ouster_clib/ouster_types.h"
#include "ouster_clib/ouster_meta.h"

#ifdef __cplusplus
extern "C" {
#endif

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


void ouster_lidar_header_get(char const *buf, ouster_lidar_header_t *dst);

void ouster_column_get1(char const *colbuf, void *dst, int type);

void ouster_column_get(char const *colbuf, ouster_column_t *dst);

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_LIDAR_H