#pragma once
#include <flecs.h>

typedef struct
{
    ecs_i32_t dummy;
} RenderingsContext;
typedef struct
{
    ecs_i32_t dummy;
} RenderingsDraw;

extern ECS_COMPONENT_DECLARE(RenderingsContext);
extern ECS_COMPONENT_DECLARE(RenderingsDraw);




void RenderingsImport(ecs_world_t *world);