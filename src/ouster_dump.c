#include "ouster_clib/ouster_dump.h"
#include "ouster_clib/ouster_assert.h"
#include <string.h>

void ouster_dump_lidar_header(FILE *f, ouster_lidar_header_t const *p)
{
	ouster_assert_notnull(f);
	ouster_assert_notnull(p);
	fprintf(f, "type=%ji, frame=%ji, init=%ji, prod=%ji, countdown_thermal_shutdown=%ji, countdown_shot_limiting=%ji, thermal_shutdown=%ji, shot_limiting=%ji\n",
	        (intmax_t)p->packet_type,
	        (intmax_t)p->frame_id,
	        (intmax_t)p->init_id,
	        (intmax_t)p->prod_sn,
	        (intmax_t)p->countdown_thermal_shutdown,
	        (intmax_t)p->countdown_shot_limiting,
	        (intmax_t)p->thermal_shutdown,
	        (intmax_t)p->shot_limiting);
}

void ouster_dump_column(FILE *f, ouster_column_t const *column)
{
	ouster_assert_notnull(f);
	ouster_assert_notnull(column);
	fprintf(f, "ts=%ji, status=%ji, mid=%ji\n", (intmax_t)column->ts, (intmax_t)column->status, (intmax_t)column->mid);
}

void ouster_dump_meta(FILE * f, ouster_meta_t const *meta)
{
	ouster_assert_notnull(f);
	ouster_assert_notnull(meta);
	fprintf(f, "ouster_meta_t dump:\n");
	fprintf(f, "%40s: %i\n", "(Azimuth columns start) mid0", meta->mid0);
	fprintf(f, "%40s: %i\n", "(Azimuth columns end) mid1", meta->mid1);
	fprintf(f, "%40s: %i\n", "(Azimuth columns width) midw", meta->midw);
	fprintf(f, "%40s: %i\n", "udp_port_lidar", meta->udp_port_lidar);
	fprintf(f, "%40s: %i\n", "udp_port_imu", meta->udp_port_imu);
	fprintf(f, "%40s: %i\n", "columns_per_frame", meta->columns_per_frame);
	fprintf(f, "%40s: %i\n", "columns_per_packet", meta->columns_per_packet);
	fprintf(f, "%40s: %i\n", "pixels_per_column", meta->pixels_per_column);
	fprintf(f, "%40s: %iB\n", "channel_data_size", meta->channel_data_size);
	fprintf(f, "%40s: %i\n", "profile", meta->profile);
	fprintf(f, "%40s: %i\n", "channel_data_size", meta->channel_data_size);
	fprintf(f, "%40s: %iB\n", "col_size", meta->col_size);
	fprintf(f, "%40s: %iB\n", "lidar_packet_size", meta->lidar_packet_size);
} 