#include "ouster_app.h"





void *thread_receiver(void *arg)
{
	ouster_app_t *app = arg;

	ouster_lidar_t lidar = {0};

	while (1)
	{
		char buf[NET_UDP_MAX_SIZE];
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = net_select(app->socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0)
		{
			platform_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR))
		{
			int64_t n = net_read(app->socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			platform_log("%-10s %5ji:  \n", "SOCK_LIDAR", (intmax_t)n);
			if (n <= 0)
			{
				continue;
			}
			ouster_lidar_get_fields(&lidar, &app->meta, buf, app->fields0, FIELD_COUNT);
			// printf("%i %i\n", lidar.last_mid, sensor->meta.mid1);
			if (lidar.last_mid == app->meta.mid1)
			{
				ecs_os_mutex_lock(app->lock);
				ouster_field_cpy(app->fields1, app->fields0, FIELD_COUNT);
				ecs_os_mutex_unlock(app->lock);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU))
		{
			int64_t n = net_read(app->socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			platform_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
		}
	}
}




ouster_app_t * ouster_app_init(ouster_app_desc_t * desc)
{
	ouster_app_t * app = ecs_os_calloc_t(ouster_app_t);
	app->socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(desc->hint_lidar ? desc->hint_lidar : "7502");
	app->socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_lidar(desc->hint_imu ? desc->hint_imu : "7503");

	app->lock = ecs_os_mutex_new();

	char *content = fs_readfile(desc->metafile);
	ouster_meta_parse(content, &app->meta);
	free(content);

	ouster_lut_init(&app->lut, &app->meta);
	platform_log("Column window: %i %i\n", app->meta.mid0, app->meta.mid1);

	app->fields0[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
	app->fields0[FIELD_RANGE].depth = 4;

	app->fields0[FIELD_IR].quantity = OUSTER_QUANTITY_NEAR_IR;
	app->fields0[FIELD_IR].depth = 4;

	app->fields1[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
	app->fields1[FIELD_RANGE].depth = 4;

	app->fields1[FIELD_IR].quantity = OUSTER_QUANTITY_NEAR_IR;
	app->fields1[FIELD_IR].depth = 4;

	app->fields2[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
	app->fields2[FIELD_RANGE].depth = 4;

	app->fields2[FIELD_IR].quantity = OUSTER_QUANTITY_NEAR_IR;
	app->fields2[FIELD_IR].depth = 4;

	ouster_field_init(app->fields0, FIELD_COUNT, &app->meta);
	ouster_field_init(app->fields1, FIELD_COUNT, &app->meta);
	ouster_field_init(app->fields2, FIELD_COUNT, &app->meta);
	
	int cap = app->lut.w * app->lut.h;
	app->image_points_size = cap * sizeof(double) * 3;
	app->image_points_data = ecs_os_calloc(app->image_points_size);

	app->thread = ecs_os_thread_new(thread_receiver, app);
	return app;
}






