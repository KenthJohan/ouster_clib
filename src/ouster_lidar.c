#include "ouster_clib/ouster_lidar.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_log.h"
#include <string.h>

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

void ouster_column_get1(char const *colbuf, void *dst, int type)
{
	ouster_assert_notnull(colbuf);
	ouster_assert_notnull(dst);
	switch (type) {
	case ouster_id(ouster_timestamp_t):
		memcpy(dst, colbuf + 0, sizeof(ouster_timestamp_t));
		break;
	case ouster_id(ouster_measurment_id_t):
		memcpy(dst, colbuf + 8, sizeof(ouster_measurment_id_t));
		break;
	case ouster_id(ouster_status_t):
		memcpy(dst, colbuf + 10, sizeof(ouster_status_t));
		*((ouster_status_t *)dst) &= 0xffff;
		break;
	default:
		ouster_assert(0, "");
		break;
	}
}

void ouster_column_get(char const *colbuf, ouster_column_t *dst)
{
	ouster_assert_notnull(colbuf);
	ouster_assert_notnull(dst);
	ouster_column_get1(colbuf, &dst->ts, ouster_id(ouster_timestamp_t));
	ouster_column_get1(colbuf, &dst->status, ouster_id(ouster_status_t));
	ouster_column_get1(colbuf, &dst->mid, ouster_id(ouster_measurment_id_t));
}

void pxcpy(char *dst, int dst_inc, char const *src, int src_inc, int n, int esize)
{
	ouster_assert_notnull(dst);
	ouster_assert_notnull(src);
	char *d = dst;
	char const *s = src;
	for (int i = 0; i < n; i++, d += dst_inc, s += src_inc) {
		memcpy(d, s, esize);
	}
}

void field_copy(ouster_field_t *field, ouster_meta_t *meta, int mid, char const *pxbuf)
{
	ouster_assert_notnull(field);
	ouster_assert_notnull(meta);
	ouster_assert_notnull(pxbuf);
	// Row major - each row is continuous memory
	char *data = field->data;
	ouster_extract_t *extract = meta->extract + field->quantity;
	int depth = extract->depth;
	int offset = extract->offset;
	int rowsize = field->rowsize;
	int rows = meta->pixels_per_column;
	int mid0 = meta->mid0;
	int mid1 = meta->mid1;
	ouster_assert(mid <= mid1, "Incorrect mid=%i (Column Measurement Id) not in range %i to %i", mid, mid0, mid1);
	char *dst = data + (mid - mid0) * field->depth;
	pxcpy(dst, rowsize, pxbuf + offset, meta->channel_data_size, rows, depth);
}

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount)
{
	ouster_assert_notnull(lidar);
	ouster_assert_notnull(buf);
	ouster_assert(fields || ((fields == NULL) && (fcount == 0)), "");

	char const *colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
	ouster_lidar_header_t header = {0};
	ouster_column_t column = {0};
	ouster_lidar_header_get(buf, &header);
	// ouster_lidar_header_dump(&header);
	ouster_column_get(colbuf, &column);

	if (lidar->frame_id != (int)header.frame_id) {
		// ouster_log("New Frame!\n");
		lidar->frame_id = (int)header.frame_id;
		// lidar->last_mid = 0;
		lidar->last_mid = column.mid - 1;
		lidar->mid_loss = 0;
		lidar->num_valid_pixels = 0;
	}

	int mid_delta = column.mid - lidar->last_mid;
	// ouster_log("mid_delta %i\n", mid_delta);
	lidar->mid_loss += (mid_delta - 1);

	// col_size = 1584
	for (int icol = 0; icol < meta->columns_per_packet; icol++, colbuf += meta->col_size) {
		ouster_column_get(colbuf, &column);
		// ouster_dump_column(stdout, &column);

		if ((column.status & 0x01) == 0) {
			continue;
		}
		char const *pxbuf = colbuf + OUSTER_COLUMN_HEADER_SIZE;

		for (int j = 0; j < fcount; ++j) {
			ouster_assert(fields[j].cols > 0, "");
			ouster_assert(fields[j].rows > 0, "");
			ouster_assert(fields[j].depth > 0, "");
			field_copy(fields + j, meta, column.mid, pxbuf);
			lidar->num_valid_pixels += meta->pixels_per_column;
		}
		lidar->last_mid = column.mid;
	}

	for (int j = 0; j < fcount; ++j) {
		// TODO: Refactor this mask procedure
		ouster_field_apply_mask_u32(fields + j, meta);
	}

	for (int j = 0; j < fcount; ++j) {
		ouster_assert_notnull(fields[j].data);
		ouster_assert(fields[j].cols > 0, "");
		ouster_assert(fields[j].rows > 0, "");
		ouster_assert(fields[j].depth > 0, "");
	}
}