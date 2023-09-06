#pragma once
#include <flecs.h>

typedef struct
{
    float pos[3];
    float mvp[16];
} Camera;


extern ECS_COMPONENT_DECLARE(Camera);

void CamerasImport(ecs_world_t *world);