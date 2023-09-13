#include "Sensors.h"

#include <ouster_clib/sock.h>
#include <ouster_clib/client.h>
#include <ouster_clib/types.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lut.h>

#include <platform/log.h>
#include <platform/net.h>
#include <platform/fs.h>

#include <viz/Geometries.h>
#include <viz/Pointclouds.h>

#include <easymath/lin_f32.h>

#include <flecs.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef enum
{
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

typedef enum
{
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef struct
{
	ecs_os_thread_t thread; // Receives UDP packages from LiDAR sensor
	ecs_os_mutex_t lock;	// Used for thread safe pointcloud copy from socket thread to main thread
	int image_size;
	double *image_points;  // Thread producer, uses (lock)
	double *image_points2; // Thread consumer, uses (lock)
	ouster_lut_t lut;
	ouster_meta_t meta;
} OusterSensor;

ECS_COMPONENT_DECLARE(OusterSensor);
ECS_COMPONENT_DECLARE(OusterSensorDesc);

void *thread_receiver(void *arg)
{
	OusterSensor *sensor = arg;

	ouster_lidar_t lidar = {0};

	int socks[SOCK_INDEX_COUNT] = {0};
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar("7502");
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_lidar("7503");

	ouster_field_t fields[FIELD_COUNT];
	fields[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
	ouster_field_init(fields, FIELD_COUNT, &sensor->meta);

	while (1)
	{
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0)
		{
			platform_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR))
		{
			char buf[NET_UDP_MAX_SIZE];
			int64_t n = net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			platform_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
			if (n <= 0)
			{
				continue;
			}
			ouster_lidar_get_fields(&lidar, &sensor->meta, buf, fields, FIELD_COUNT);
			if (lidar.last_mid == sensor->meta.mid1)
			{
				ecs_os_mutex_lock(sensor->lock);
				ouster_lut_cartesian(&sensor->lut, fields[FIELD_RANGE].data, sensor->image_points);
				ecs_os_mutex_unlock(sensor->lock);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU))
		{
			char buf[1024 * 256];
			int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			platform_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
		}
	}
}

void Pointcloud_copy(Pointcloud *cloud, double const *src, int n, double filter_radius)
{
	int k = 0;
	float *dst = cloud->pos;
	for (int j = 0; j < n; ++j, src += 3)
	{
		float d = sqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
		if (d < filter_radius)
		{
			continue;
		}
		dst[0] = src[0] * 0.01;
		dst[1] = src[1] * 0.01;
		dst[2] = src[2] * 0.01;
		dst += 3;
		k++;
		// printf("xyz %f %f %f\n", dst[0], dst[1], dst[2]);
	}
	cloud->count = k;
}

void Pointcloud_Fill(ecs_iter_t *it)
{
	Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
	OusterSensor *sensor = ecs_field(it, OusterSensor, 2);
	OusterSensorDesc *desc = ecs_field(it, OusterSensorDesc, 3);
	for (int i = 0; i < it->count; ++i, ++cloud, ++sensor)
	{
		ecs_os_mutex_lock(sensor->lock);
		ecs_os_memcpy(sensor->image_points2, sensor->image_points, sensor->image_size);
		ecs_os_mutex_unlock(sensor->lock);
		Pointcloud_copy(cloud, sensor->image_points2, sensor->lut.w * sensor->lut.h, desc->radius_filter);
	}
}

static void OusterSensor_Add(ecs_iter_t *it)
{
	OusterSensorDesc *desc = ecs_field(it, OusterSensorDesc, 1);
	for (int i = 0; i < it->count; ++i, ++desc)
	{
		OusterSensor *sensor = ecs_get_mut(it->world, it->entities[i], OusterSensor);
		sensor->lock = ecs_os_mutex_new();
		char *content = fs_readfile(desc->metafile);
		ouster_meta_parse(content, &sensor->meta);
		free(content);
		ouster_lut_init(&sensor->lut, &sensor->meta);
		printf("Column window: %i %i\n", sensor->meta.mid0, sensor->meta.mid1);
		int cap = sensor->lut.w * sensor->lut.h;
		sensor->image_size = cap * sizeof(double) * 3;
		sensor->image_points = ecs_os_calloc(sensor->image_size);
		sensor->image_points2 = ecs_os_calloc(sensor->image_size);
		ecs_set(it->world, it->entities[i], Pointcloud, {.cap = cap, .count = cap});
		sensor->thread = ecs_os_thread_new(thread_receiver, sensor);
	}
}

void SensorsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Sensors);
	ECS_IMPORT(world, Geometries);
	ECS_IMPORT(world, Pointclouds);

	ECS_COMPONENT_DEFINE(world, OusterSensor);
	ECS_COMPONENT_DEFINE(world, OusterSensorDesc);

	ecs_struct(world, {.entity = ecs_id(OusterSensorDesc),
					   .members = {
						   {.name = "metafile", .type = ecs_id(ecs_string_t)},
						   {.name = "radius_filter", .type = ecs_id(ecs_f64_t)},
					   }});

	ecs_system_init(world, &(ecs_system_desc_t){
							   .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
							   .callback = OusterSensor_Add,
							   .query.filter.terms =
								   {
									   {.id = ecs_id(OusterSensorDesc), .src.flags = EcsSelf},
									   {.id = ecs_id(OusterSensor), .oper = EcsNot}, // Adds this
								   }});

	ecs_system_init(world, &(ecs_system_desc_t){
							   .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
							   .callback = Pointcloud_Fill,
							   .query.filter.terms =
								   {
									   {.id = ecs_id(Pointcloud), .src.flags = EcsSelf},
									   {.id = ecs_id(OusterSensor), .src.flags = EcsSelf},
									   {.id = ecs_id(OusterSensorDesc), .src.flags = EcsSelf},
									   //{ .id = ecs_id(OusterSensor), .src.trav = EcsIsA, .src.flags = EcsUp},
								   }});
}