#pragma once
#include <flecs.h>

typedef struct
{
    ecs_world_t * world;
    float rx;
    float ry;
} glid_state_t;

void glid_init(glid_state_t * state);