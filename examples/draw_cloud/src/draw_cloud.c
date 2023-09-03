#include <glid/glid.h>
#include <glid/ModPointcloud.h>
#include <flecs.h>

int main(int argc, char* argv[])
{
    ecs_world_t * world = ecs_init_w_args(argc, argv);


    ECS_IMPORT(world, ModPointcloud);

    ecs_entity_t e = ecs_set(world, 0, Pointcloud, {10, 20});
    ecs_set(world, e, Pointcloud, {1, 2});

    
    glid_state_t state = 
    {
        .world = world
    };
    glid_init(&state);


    return ecs_fini(world);
}