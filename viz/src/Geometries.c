#include "viz/Geometries.h"


ECS_COMPONENT_DECLARE(Pointcloud);



ECS_CTOR(Pointcloud, ptr, {
    ecs_os_memset_t(ptr, 0, Pointcloud);
})

ECS_DTOR(Pointcloud, ptr, {
    ecs_os_free(ptr->pos);
    ecs_os_free(ptr->col);
    ecs_os_memset_t(ptr, 0, Pointcloud);
})

ECS_MOVE(Pointcloud, dst, src, {
    ecs_os_free(dst->pos);
    ecs_os_free(dst->col);
    ecs_os_memcpy_t(dst, src, Pointcloud);
    ecs_os_memset_t(src, 0, Pointcloud);
})

ECS_COPY(Pointcloud, dst, src, {
    ecs_os_free(dst->pos);
    ecs_os_free(dst->col);
    dst->pos = ecs_os_calloc(src->count * sizeof(float) * 3);
    dst->col = ecs_os_calloc(src->count * sizeof(float) * 4);
    ecs_os_memcpy(dst->pos, src->pos, src->count * sizeof(float) * 3);
    ecs_os_memcpy(dst->col, src->col, src->count * sizeof(float) * 4);
})



void Observer(ecs_iter_t *it)
{
    Pointcloud *p = ecs_field(it, Pointcloud, 1);
    ecs_os_memset_n(p, 0, Pointcloud, it->count);
    for(int i = 0; i < it->count; ++i, p++)
    {
        p->count = 300;
        p->pos = ecs_os_calloc(p->count * sizeof(float) * 3);
        p->col = ecs_os_calloc(p->count * sizeof(float) * 4);
    }
}



void GeometriesImport(ecs_world_t *world)
{
    ECS_MODULE(world, Geometries);
    ECS_COMPONENT_DEFINE(world, Pointcloud);
    ECS_OBSERVER(world, Observer, EcsOnSet, Pointcloud);


    ecs_set_hooks(world, Pointcloud, {
        .ctor = ecs_ctor(Pointcloud),
        .move = ecs_move(Pointcloud),
        .copy = ecs_copy(Pointcloud),
        .dtor = ecs_dtor(Pointcloud),
    });


}