#include "ouster_clib/lidar.h"
#include "ouster_clib/lidar_header.h"
#include "ouster_clib/lidar_column.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_assert.h"
#include <string.h>

void pxcpy(char *dst, int dst_inc, char const *src, int src_inc, int n, int esize)
{
	ouster_assert_notnull(dst);
	ouster_assert_notnull(src);
	char *d = dst;
	char const *s = src;
	for (int i = 0; i < n; i++, d += dst_inc, s += src_inc)
	{
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
	ouster_extract_t * extract = meta->extract + field->quantity;
	int depth = extract->depth;
	int offset = extract->offset;
	int rowsize = field->rowsize;
	int rows = meta->pixels_per_column;
	int mid0 = meta->mid0;
	int mid1 = meta->mid1;
	ouster_assert(mid <= mid1, "");
	char *dst = data + (mid - mid0) * field->depth;
	pxcpy(dst, rowsize, pxbuf + offset, meta->channel_data_size, rows, depth);
}

void ouster_lidar_get_fields(ouster_lidar_t *lidar, ouster_meta_t *meta, char const *buf, ouster_field_t *fields, int fcount)
{
	ouster_assert_notnull(lidar);
	ouster_assert_notnull(buf);
	ouster_assert_notnull(fields);

	char const *colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
	ouster_lidar_header_t header = {0};
	ouster_column_t column = {0};
	ouster_lidar_header_get(buf, &header);
	// ouster_lidar_header_log(&header);
	ouster_column_get(colbuf, &column);

	if (lidar->frame_id != (int)header.frame_id)
	{
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
	for (int icol = 0; icol < meta->columns_per_packet; icol++, colbuf += meta->col_size)
	{
		ouster_column_get(colbuf, &column);
		// ouster_column_log(&column);

		if ((column.status & 0x01) == 0)
		{
			continue;
		}
		char const *pxbuf = colbuf + OUSTER_COLUMN_HEADER_SIZE;

		for (int j = 0; j < fcount; ++j)
		{
			ouster_assert(fields[j].cols > 0, "");
			ouster_assert(fields[j].rows > 0, "");
			ouster_assert(fields[j].depth > 0, "");
			field_copy(fields + j, meta, column.mid, pxbuf);
			lidar->num_valid_pixels += meta->pixels_per_column;
		}
		lidar->last_mid = column.mid;
	}

	for (int j = 0; j < fcount; ++j)
	{
		// TODO: Refactor this mask procedure
		ouster_field_apply_mask_u32(fields + j, meta);
	}

	for (int j = 0; j < fcount; ++j)
	{
		ouster_assert_notnull(fields[j].data);
		ouster_assert(fields[j].cols > 0, "");
		ouster_assert(fields[j].rows > 0, "");
		ouster_assert(fields[j].depth > 0, "");
	}
}