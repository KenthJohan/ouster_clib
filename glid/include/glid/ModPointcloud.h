#pragma once
#include <flecs.h>

typedef struct
{
    double x, y;
} Pointcloud;


extern ECS_COMPONENT_DECLARE(Pointcloud);


void ModPointcloudImport(ecs_world_t *world);