#pragma once
#include <flecs.h>

typedef struct
{
	const ecs_string_t metafile;
	double radius_filter;
} OusterSensorDesc;

extern ECS_COMPONENT_DECLARE(OusterSensorDesc);

void SensorsImport(ecs_world_t *world);