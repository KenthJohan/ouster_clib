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

extern ECS_COMPONENT_DECLARE(Window);
extern ECS_COMPONENT_DECLARE(RenderingsContext);

void WindowsImport(ecs_world_t *world);