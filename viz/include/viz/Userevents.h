#pragma once
#include <flecs.h>




typedef struct
{
    int keys[128];
} UsereventsInput;


extern ECS_COMPONENT_DECLARE(UsereventsInput);


void UsereventsImport(ecs_world_t *world);