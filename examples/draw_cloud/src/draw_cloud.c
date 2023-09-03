#include <glid/glid.h>
#include <glid/ModPointcloud.h>
#include <flecs.h>

int main(int argc, char* argv[])
{
    ecs_world_t * world = ecs_init_w_args(argc, argv);


    ECS_IMPORT(world, ModPointcloud);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add(world, e, Pointcloud);


    glid_state_t state = 
    {
        .world = world
    };
    glid_init(&state);


    return ecs_fini(world);
}