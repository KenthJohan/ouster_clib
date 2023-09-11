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




extern ECS_COMPONENT_DECLARE(Position2);
extern ECS_COMPONENT_DECLARE(Position3);

void GeometriesImport(ecs_world_t *world);