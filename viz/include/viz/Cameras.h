#pragma once
#include <flecs.h>

typedef struct
{
    float mvp[16]; // For shader
    float proj[16]; // Projection matrix

    float q[4]; // Orientation quaternion

    float pos[3]; // Position

    float move[3]; // 
    float look[3];
} Camera;


extern ECS_COMPONENT_DECLARE(Camera);

void CamerasImport(ecs_world_t *world);