#include <glid/glid.h>
#include <glid/ModPointcloud.h>
#include <flecs.h>





void SysUpdateColor(ecs_iter_t *it)
{
    Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
    ecs_os_sleep(1,0);
    for(int i = 0; i < it->count; ++i, cloud++)
    {
        char * data_col = cloud->col;
        for(int j = 0; j < cloud->count; ++j, data_col += cloud->col_step)
        {
            double * col = (void*)data_col;
        }
    }
}






int main(int argc, char* argv[])
{
    ecs_world_t * world = ecs_init_w_args(argc, argv);
    ecs_set_threads(world, 4);

    ECS_IMPORT(world, ModPointcloud);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = SysUpdateColor,
        .query.filter = {
            .expr = "Pointcloud"
        },
        .multi_threaded = false,
        .no_readonly = true
    });

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add(world, e, Pointcloud);


    glid_state_t state = 
    {
        .world = world
    };
    glid_init(&state);


    return ecs_fini(world);
}