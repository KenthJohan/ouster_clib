#pragma once
#include <flecs.h>


typedef struct
{
    ecs_i32_t cap_vertices;
    ecs_i32_t cap_indices;
} ShapeBuffer;

typedef struct
{
    ecs_f32_t r;
    ecs_i32_t slices;
    ecs_i32_t stacks;
} Sphere;

typedef struct
{
    ecs_f32_t x;
    ecs_f32_t y;
} Position2;

typedef struct
{
    ecs_f32_t x;
    ecs_f32_t y;
    ecs_f32_t z;
} Position3;


typedef struct
{
	ecs_vec_t pos; //vec<Position3>
} GeometriesInstances;


extern ECS_COMPONENT_DECLARE(ShapeBuffer);
extern ECS_COMPONENT_DECLARE(Sphere);
extern ECS_COMPONENT_DECLARE(Position2);
extern ECS_COMPONENT_DECLARE(Position3);
extern ECS_COMPONENT_DECLARE(GeometriesInstances);

void GeometriesImport(ecs_world_t *world);