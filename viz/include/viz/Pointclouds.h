#pragma once
#include <flecs.h>

typedef struct
{
	int cap;
	int count;
	float *pos;	   // xyz
	float *vel;	   // xyz
	uint32_t *col; // rgba
} Pointcloud;

extern ECS_COMPONENT_DECLARE(Pointcloud);

void PointcloudsImport(ecs_world_t *world);