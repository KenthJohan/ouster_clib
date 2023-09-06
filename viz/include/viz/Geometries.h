#pragma once
#include <flecs.h>





typedef struct
{
    float * pos; //xyz
    float * col; //rgba
    int count;
} Pointcloud;


extern ECS_COMPONENT_DECLARE(Pointcloud);
extern ecs_entity_t EntRenderPointcloud;

void GeometriesImport(ecs_world_t *world);