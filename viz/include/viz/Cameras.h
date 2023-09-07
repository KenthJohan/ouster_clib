#pragma once
#include <flecs.h>

typedef struct
{
    float mvp[16];
    float proj[16];

    float q[4];

    float pos[3];

    float move[3];
    float look[3];
} Camera;


extern ECS_COMPONENT_DECLARE(Camera);

void CamerasImport(ecs_world_t *world);