#pragma once
#include <flecs.h>

typedef struct
{
	ecs_i32_t cap;
} DrawShapesDesc;

extern ECS_COMPONENT_DECLARE(DrawShapesDesc);

void DrawShapesImport(ecs_world_t *world);