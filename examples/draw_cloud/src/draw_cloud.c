#include <viz/viz.h>
#include <viz/Geometries.h>
#include <viz/Renderings.h>
#include <viz/Cameras.h>
#include <viz/DrawText.h>

#include "Sensors.h"

#include <stdio.h>
#include <assert.h>

#include <platform/fs.h>
#include <platform/log.h>





int main(int argc, char* argv[])
{
    fs_pwd();
    
    if(argc <= 1)
    {
        printf("Missing input meta file\n");
        return 0;
    }




    ecs_world_t * world = ecs_init();


    // Must import before loading flecs file
    ECS_IMPORT(world, Geometries);
    ECS_IMPORT(world, Cameras);
    ECS_IMPORT(world, Renderings);
    ECS_IMPORT(world, DrawText);

    //https://www.flecs.dev/explorer/?remote=true
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	ecs_plecs_from_file(world, "profile.flecs");
    //ecs_set_threads(world, 4);


    //ECS_IMPORT(world, Sensors);





    while(0)
    {
        ecs_progress(world, 0);
    }

    viz_state_t state = 
    {
        .world = world
    };
    viz_init(&state);
    viz_run(&state);


    return ecs_fini(world);
}