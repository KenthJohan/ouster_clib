#pragma once
#include <flecs.h>


typedef struct
{
    ecs_i32_t w;
    ecs_i32_t h;
    ecs_f32_t dt;
    ecs_string_t title;
} Window;

typedef struct
{
    ecs_i32_t dummy;
} RenderingsContext;

typedef struct
{
    ecs_i32_t dummy;
} Draw;

extern ECS_COMPONENT_DECLARE(Window);
extern ECS_COMPONENT_DECLARE(RenderingsContext);
extern ECS_COMPONENT_DECLARE(Draw);

extern ECS_TAG_DECLARE(Update);
extern ECS_TAG_DECLARE(Invalid);
extern ECS_TAG_DECLARE(Valid);
extern ECS_TAG_DECLARE(Setup);

void WindowsImport(ecs_world_t *world);