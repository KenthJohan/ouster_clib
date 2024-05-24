#include "rxlidar.h"

void *rec(app_t *app)
{
	ouster_meta_t *meta = &app->meta;

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, meta);

	int socks[2];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(7502, OUSTER_DEFAULT_RCVBUF_SIZE);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(7503, OUSTER_DEFAULT_RCVBUF_SIZE);

	ouster_lidar_t lidar = {0};

	ouster_lut_t lut = {0};
	ouster_lut_init(&lut, meta);
	double *xyz = calloc(1, lut.w * lut.h * sizeof(double) * 3);
	app->points_xyz = calloc(1, lut.w * lut.h * sizeof(double) * 3);
	app->points_count = lut.w * lut.h;

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = ouster_net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char buf[OUSTER_NET_UDP_MAX_SIZE];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			if (n == meta->lidar_packet_size) {
				ouster_lidar_get_fields(&lidar, meta, buf, fields, FIELD_COUNT);
				if (lidar.last_mid == meta->mid1) {
					ouster_lut_cartesian_f64(&lut, fields[FIELD_RANGE].data, xyz, sizeof(double)*3);
					ouster_field_zero(fields, FIELD_COUNT);
					//printf("frame=%i, mid_loss=%i\n", lidar.frame_id, lidar.mid_loss);

					pthread_mutex_lock(&app->lock);
					memcpy(app->points_xyz, xyz, app->points_count*sizeof(double)*3);
					pthread_mutex_unlock(&app->lock);

				}
			} else {
				printf("Bytes received (%ji) does not match lidar_packet_size (%ji)\n", (intmax_t)n, (intmax_t)app->meta.lidar_packet_size);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			// char buf[OUSTER_NET_UDP_MAX_SIZE];
			// int64_t n = ouster_net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
		}
	}

	return NULL;
}
