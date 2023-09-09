#include "viz/Geometries.h"
#include <assert.h>

ECS_COMPONENT_DECLARE(Position3);
ECS_COMPONENT_DECLARE(Pointcloud);



ECS_CTOR(Pointcloud, ptr, {
    ecs_os_memset_t(ptr, 0, Pointcloud);
})

ECS_DTOR(Pointcloud, ptr, {
    ecs_os_free(ptr->pos);
    ecs_os_free(ptr->col);
})

ECS_MOVE(Pointcloud, dst, src, {
    ecs_os_free(dst->pos);
    ecs_os_free(dst->col);
    ecs_os_memcpy_t(dst, src, Pointcloud);
    ecs_os_memset_t(src, 0, Pointcloud);
    src->pos = NULL;
    src->col = NULL;
})




void Observer(ecs_iter_t *it)
{
    Pointcloud *p = ecs_field(it, Pointcloud, 1);
    for(int i = 0; i < it->count; ++i, p++)
    {
        assert(p->count > 0);
        assert(p->count < 100000);
        p->pos = ecs_os_calloc(p->count * sizeof(float) * 3);
        p->col = ecs_os_calloc(p->count * sizeof(float) * 4);
    }
}



void GeometriesImport(ecs_world_t *world)
{
    ECS_MODULE(world, Geometries);
    ECS_COMPONENT_DEFINE(world, Position3);
    ECS_COMPONENT_DEFINE(world, Pointcloud);
    ECS_OBSERVER(world, Observer, EcsOnSet, Pointcloud);


    ecs_set_hooks(world, Pointcloud, {
        .ctor = ecs_ctor(Pointcloud),
        .move = ecs_move(Pointcloud),
        .dtor = ecs_dtor(Pointcloud),
    });

	ecs_struct(world, {
	.entity = ecs_id(Position3),
	.members = {
	{ .name = "x", .type = ecs_id(ecs_f32_t) },
	{ .name = "y", .type = ecs_id(ecs_f32_t) },
	{ .name = "z", .type = ecs_id(ecs_f32_t) },
	}
	});

}