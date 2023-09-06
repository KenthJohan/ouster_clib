#pragma once
#include <flecs.h>

typedef struct
{
    ecs_world_t * world;
    float rx;
    float ry;
} viz_state_t;

void viz_init(viz_state_t * state);
void viz_run(viz_state_t * state);