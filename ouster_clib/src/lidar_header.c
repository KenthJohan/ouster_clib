#include "ouster_clib/lidar_header.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_log.h"
#include <string.h>

void ouster_lidar_header_log(ouster_lidar_header_t *p)
{
	ouster_assert_notnull(p);
	ouster_log("type=%ji, frame=%ji, init=%ji, prod=%ji, countdown_thermal_shutdown=%ji, countdown_shot_limiting=%ji, thermal_shutdown=%ji, shot_limiting=%ji\n",
	           (intmax_t)p->packet_type,
	           (intmax_t)p->frame_id,
	           (intmax_t)p->init_id,
	           (intmax_t)p->prod_sn,
	           (intmax_t)p->countdown_thermal_shutdown,
	           (intmax_t)p->countdown_shot_limiting,
	           (intmax_t)p->thermal_shutdown,
	           (intmax_t)p->shot_limiting);
}

void ouster_lidar_header_get1(char const *buf, void *dst, int type)
{
	ouster_assert_notnull(buf);
	ouster_assert_notnull(dst);
	switch (type) {
	case ouster_id(ouster_packet_type_t):
		memcpy(dst, buf + 0, sizeof(ouster_packet_type_t));
		break;
	case ouster_id(ouster_frame_id_t):
		memcpy(dst, buf + 2, sizeof(ouster_frame_id_t));
		break;
	case ouster_id(ouster_init_id_t):
		memcpy(dst, buf + 4, sizeof(ouster_init_id_t));
		*((ouster_init_id_t *)dst) &= UINT32_C(0x00ffffff);
		break;
	case ouster_id(ouster_prod_sn_t):
		memcpy(dst, buf + 7, sizeof(ouster_prod_sn_t));
		*((ouster_prod_sn_t *)dst) &= UINT64_C(0x000000ffffffffff);
		break;
	case ouster_id(ouster_countdown_thermal_shutdown_t):
		memcpy(dst, buf + 16, sizeof(ouster_countdown_thermal_shutdown_t));
		break;
	case ouster_id(ouster_countdown_shot_limiting_t):
		memcpy(dst, buf + 17, sizeof(ouster_countdown_shot_limiting_t));
		break;
	case ouster_id(ouster_thermal_shutdown_t):
		memcpy(dst, buf + 18, sizeof(ouster_thermal_shutdown_t));
		break;
	case ouster_id(ouster_shot_limiting_t):
		memcpy(dst, buf + 19, sizeof(ouster_shot_limiting_t));
		break;
	default:
		ouster_assert(0, "");
		break;
	}
}

void ouster_lidar_header_get(char const *buf, ouster_lidar_header_t *dst)
{
	ouster_assert_notnull(buf);
	ouster_assert_notnull(dst);
	ouster_lidar_header_get1(buf, &dst->packet_type, ouster_id(ouster_packet_type_t));
	ouster_lidar_header_get1(buf, &dst->frame_id, ouster_id(ouster_frame_id_t));
	ouster_lidar_header_get1(buf, &dst->init_id, ouster_id(ouster_init_id_t));
	ouster_lidar_header_get1(buf, &dst->prod_sn, ouster_id(ouster_prod_sn_t));
	ouster_lidar_header_get1(buf, &dst->countdown_thermal_shutdown, ouster_id(ouster_countdown_thermal_shutdown_t));
	ouster_lidar_header_get1(buf, &dst->countdown_shot_limiting, ouster_id(ouster_countdown_shot_limiting_t));
	ouster_lidar_header_get1(buf, &dst->thermal_shutdown, ouster_id(ouster_thermal_shutdown_t));
	ouster_lidar_header_get1(buf, &dst->shot_limiting, ouster_id(ouster_shot_limiting_t));
}
