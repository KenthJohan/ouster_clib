#pragma once
#include <flecs.h>




typedef struct
{
    char * pos;
    char * col;
    int pos_step;
    int col_step;
    int count;
} Pointcloud;


extern ECS_COMPONENT_DECLARE(Pointcloud);


void ModPointcloudImport(ecs_world_t *world);