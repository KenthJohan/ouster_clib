#pragma once
#include <flecs.h>

typedef struct
{
    float mvp[16];
} Camera;


extern ECS_COMPONENT_DECLARE(Camera);

void CamerasImport(ecs_world_t *world);