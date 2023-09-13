#pragma once
#include <flecs.h>

typedef struct
{
	ecs_i32_t cap;
} DrawPointsDesc;

extern ECS_COMPONENT_DECLARE(DrawPointsDesc);

void DrawPointsImport(ecs_world_t *world);
