#pragma once
#include <flecs.h>





typedef struct
{
    float * pos; //xyz
    float * col; //rgba
    int count;
} Pointcloud;


extern ECS_COMPONENT_DECLARE(Pointcloud);


void GeometriesImport(ecs_world_t *world);