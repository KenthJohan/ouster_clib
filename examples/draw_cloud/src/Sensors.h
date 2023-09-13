#pragma once
#include <flecs.h>

typedef struct
{
	const ecs_string_t metafile;
	double radius_filter;
} SensorsDesc;

extern ECS_COMPONENT_DECLARE(SensorsDesc);

void SensorsImport(ecs_world_t *world);