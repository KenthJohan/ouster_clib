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

#include "ouster_app.h"





typedef struct
{
	ouster_app_t * app;
} SensorsState;

ECS_COMPONENT_DECLARE(SensorsState);
ECS_COMPONENT_DECLARE(SensorsDesc);



void Pointcloud_copy(Pointcloud *cloud, double const *src_pos, uint32_t const *src_ir, int n, double filter_radius)
{
	int k = 0;
	float *dst_pos = cloud->pos;
	uint32_t *dst_col = cloud->col;
	for (int j = 0; j < n; ++j, src_pos += 3, src_ir += 1)
	{
		float d = sqrt(V3_NORM2(src_pos));
		if (d < filter_radius)
		{
			continue;
		}
		dst_pos[0] = src_pos[0] * 0.01;
		dst_pos[1] = src_pos[1] * 0.01;
		dst_pos[2] = src_pos[2] * 0.01;
		int ir = src_ir[0] * 3;
		// AABBGGRR
		dst_col[0] =
			0xFF000000 |
			((ir & 0xFF) << 0) |
			((ir & 0xFF) << 8) |
			((ir & 0xFF) << 16);
		dst_pos += 3;
		dst_col += 1;
		k++;
		// printf("xyz %f %f %f\n", dst[0], dst[1], dst[2]);
	}
	cloud->count = k;
}

void Pointcloud_Fill(ecs_iter_t *it)
{
	Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
	SensorsState *sensor = ecs_field(it, SensorsState, 2);
	SensorsDesc *desc = ecs_field(it, SensorsDesc, 3);
	for (int i = 0; i < it->count; ++i, ++cloud, ++sensor)
	{
		ouster_lut_t * lut = &sensor->app->lut;
		ouster_field_t * field_range = sensor->app->fields2 + FIELD_RANGE;
		ouster_field_t * field_ir = sensor->app->fields2 + FIELD_IR;
		double *  image_points_data = sensor->app->image_points_data;

		ecs_os_mutex_lock(sensor->app->lock);
		ouster_field_cpy(sensor->app->fields2, sensor->app->fields1, FIELD_COUNT);
		ecs_os_mutex_unlock(sensor->app->lock);

		ouster_lut_cartesian_f64(lut, field_range->data, image_points_data, 3);
		int n = sensor->app->lut.w * sensor->app->lut.h;
		Pointcloud_copy(cloud, image_points_data, field_ir->data, n, desc->radius_filter);
	}
}

static void Sensor_Add(ecs_iter_t *it)
{
	SensorsDesc *desc = ecs_field(it, SensorsDesc, 1);
	for (int i = 0; i < it->count; ++i, ++desc)
	{
		SensorsState *sensor = ecs_get_mut(it->world, it->entities[i], SensorsState);
		sensor->app = ouster_app_init(&(ouster_app_desc_t){
			.metafile = desc->metafile,
			.hint_imu = NULL,
			.hint_lidar = NULL,
			.fields = {.q = {OUSTER_QUANTITY_RANGE, OUSTER_QUANTITY_NEAR_IR}}
			});
		int count = sensor->app->lut.w * sensor->app->lut.h;
		ecs_set(it->world, it->entities[i], Pointcloud, {.cap = count, .count = count, .point_size = 0.1f});
	}
}

void SensorsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Sensors);
	ECS_IMPORT(world, Geometries);
	ECS_IMPORT(world, Pointclouds);

	ECS_COMPONENT_DEFINE(world, SensorsState);
	ECS_COMPONENT_DEFINE(world, SensorsDesc);

	ecs_struct(world, {.entity = ecs_id(SensorsDesc),
		.members = {
		{.name = "metafile", .type = ecs_id(ecs_string_t)},
		{.name = "radius_filter", .type = ecs_id(ecs_f64_t)},
		}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Sensor_Add,
		.query.filter.terms =
		{
		{.id = ecs_id(SensorsDesc), .src.flags = EcsSelf},
		{.id = ecs_id(SensorsState), .oper = EcsNot}, // Adds this
		}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Pointcloud_Fill,
		.query.filter.terms =
		{
		{.id = ecs_id(Pointcloud), .src.flags = EcsSelf},
		{.id = ecs_id(SensorsState), .src.flags = EcsSelf},
		{.id = ecs_id(SensorsDesc), .src.flags = EcsSelf},
		//{ .id = ecs_id(OusterSensor), .src.trav = EcsIsA, .src.flags = EcsUp},
		}});
}