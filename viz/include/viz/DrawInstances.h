#pragma once
#include <flecs.h>

typedef struct
{
	ecs_i32_t cap;
} DrawInstancesDesc;

extern ECS_COMPONENT_DECLARE(DrawInstancesDesc);

void DrawInstancesImport(ecs_world_t *world);