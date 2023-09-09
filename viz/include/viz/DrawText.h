#pragma once
#include <flecs.h>

typedef struct
{
    ecs_string_t content;
} Text;


extern ECS_COMPONENT_DECLARE(Text);

void DrawTextImport(ecs_world_t *world);