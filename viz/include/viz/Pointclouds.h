#pragma once
#include <flecs.h>

typedef struct
{
	ecs_i32_t cap;
	ecs_i32_t count;
	ecs_f32_t point_size;
	ecs_f32_t *pos;	   // xyz
	ecs_f32_t *vel;	   // xyz
	uint32_t *col;     // rgba
} Pointcloud;

extern ECS_COMPONENT_DECLARE(Pointcloud);

void PointcloudsImport(ecs_world_t *world);