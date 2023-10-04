#include "ouster_clib/lidar_column.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/ouster_assert.h"
#include <string.h>

void ouster_column_log(ouster_column_t const *column)
{
	ouster_assert_notnull(column);
	ouster_log("ts=%ji, status=%ji, mid=%ji\n", (intmax_t)column->ts, (intmax_t)column->status, (intmax_t)column->mid);
}

void ouster_column_get1(char const *colbuf, void *dst, int type)
{
	ouster_assert_notnull(colbuf);
	ouster_assert_notnull(dst);
	switch (type)
	{
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
