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

#include <flecs.h>

#include <stdlib.h>
#include <stdio.h>

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
	int w;
	int h;
	double *cloudimage;
	ecs_os_thread_t thread;
	ouster_lut_t lut;
	ecs_os_mutex_t lock;
	ouster_meta_t meta;
} OusterSensor;

ECS_COMPONENT_DECLARE(OusterSensor);
ECS_COMPONENT_DECLARE(OusterSensorDesc);

void *thread_receiver(void *arg)
{
	OusterSensor *sensor = arg;

	ouster_field_t fields[FIELD_COUNT] = {0};
	ouster_lidar_t lidar = {0};

	int socks[SOCK_INDEX_COUNT] = {0};
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar("7502");
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_lidar("7503");

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
			if (n <= 0)
			{
				continue;
			}
			// printf("n %ji\n", (intmax_t)n);
			ouster_lidar_get_fields(&lidar, &sensor->meta, buf, fields, FIELD_COUNT);
			if (lidar.last_mid != sensor->meta.mid1)
			{
				continue;
			}

			ouster_lut_cartesian(&sensor->lut, fields[FIELD_RANGE].data, sensor->cloudimage);
			// printf("mat = %i of %i\n", sensor->fields[0].num_valid_pixels, sensor->fields[0].mat.dim[1] * sensor->fields[0].mat.dim[2]);
			ouster_field_zero(fields, FIELD_COUNT);
			// printf("New frame %ji\n", (intmax_t)lidar.frame_id);

			/*
			cloud->count = ECS_MIN(cap, lut.w * lut.h);

			*/
		}

		if (a & (1 << SOCK_INDEX_IMU))
		{
			// char buf[1024*256];
			// int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			// platform_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
		}
	}
}

void SysUpdateColor(ecs_iter_t *it)
{
	Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
	OusterSensor *sensor = ecs_field(it, OusterSensor, 2);
	// ecs_os_sleep(1,0);
	for (int i = 0; i < it->count; ++i, ++cloud, ++sensor)
	{
		ecs_os_mutex_lock(sensor->lock);
		int n = sensor->lut.w * sensor->lut.h;
		for (int j = 0; j < n; ++j)
		{
			double *src = sensor->cloudimage + j * 3;
			float *dst = cloud->pos + j * 3;
			dst[0] = src[0] * 0.01;
			dst[1] = src[1] * 0.01;
			dst[2] = src[2] * 0.01;
			// printf("xyz %f %f %f\n", dst[0], dst[1], dst[2]);
		}
		ecs_os_mutex_unlock(sensor->lock);
	}
}

static void Observer_LidarUDP_OnAdd(ecs_iter_t *it)
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
		sensor->cloudimage = calloc(1, cap * sizeof(double) * 3);
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
					   }});

	ecs_system_init(world, &(ecs_system_desc_t){
							   .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
							   .callback = Observer_LidarUDP_OnAdd,
							   .query.filter.terms =
								   {
									   {.id = ecs_id(OusterSensorDesc), .src.flags = EcsSelf},
									   {.id = ecs_id(OusterSensor), .oper = EcsNot}, // Adds this
								   }});

	ecs_system_init(world, &(ecs_system_desc_t){
							   .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
							   .callback = SysUpdateColor,
							   .query.filter.terms =
								   {
									   {.id = ecs_id(Pointcloud), .src.flags = EcsSelf},
									   {.id = ecs_id(OusterSensor), .src.flags = EcsSelf},
									   //{ .id = ecs_id(OusterSensor), .src.trav = EcsIsA, .src.flags = EcsUp},
								   }});
}