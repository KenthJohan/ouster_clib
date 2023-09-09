#pragma once
#include <flecs.h>


typedef struct
{
    float x;
    float y;
} Position2;
typedef struct
{
    float x;
    float y;
    float z;
} Position3;

typedef struct
{
    float * pos; //xyz
    float * col; //rgba
    int count;
} Pointcloud;


extern ECS_COMPONENT_DECLARE(Position2);
extern ECS_COMPONENT_DECLARE(Position3);
extern ECS_COMPONENT_DECLARE(Pointcloud);

void GeometriesImport(ecs_world_t *world);