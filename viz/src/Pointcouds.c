#include "viz/Pointclouds.h"
#include <sokol/HandmadeMath.h>
#include <assert.h>

ECS_COMPONENT_DECLARE(Pointcloud);

#define MAX_PARTICLES (512 * 1024)
#define NUM_PARTICLES_EMITTED_PER_FRAME (100)

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

void update(hmm_vec3 *pos, hmm_vec3 *vel, int *last, int cap, int emit_per_frame, float t)
{
	// emit new particles
	for (int i = 0; i < emit_per_frame; i++)
	{
		if ((*last) < cap)
		{
			pos[(*last)] = HMM_Vec3(0.0, 0.0, 0.0);
			vel[(*last)] = HMM_Vec3(
				((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f,
				((float)(rand() & 0x7FFF) / 0x7FFF) * 0.5f + 2.0f,
				((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f);
			(*last)++;
		}
		else
		{
			break;
		}
	}

	// update particle positions
	for (int i = 0; i < (*last); i++)
	{
		vel[i].Y -= 1.0f * t;
		pos[i].X += vel[i].X * t;
		pos[i].Y += vel[i].Y * t;
		pos[i].Z += vel[i].Z * t;
		// bounce back from 'ground'
		if (pos[i].Y < -2.0f)
		{
			pos[i].Y = -1.8f;
			vel[i].Y = -vel[i].Y;
			vel[i].X *= 0.8f;
			vel[i].Y *= 0.8f;
			vel[i].Z *= 0.8f;
		}
	}
}

void Bouncer(ecs_iter_t *it)
{
	Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
	for (int i = 0; i < it->count; ++i, ++cloud)
	{
		update((hmm_vec3 *)cloud->pos, (hmm_vec3 *)cloud->vel, &cloud->count, cloud->cap, NUM_PARTICLES_EMITTED_PER_FRAME, it->delta_time);
	}
}

void Observer(ecs_iter_t *it)
{
	Pointcloud *p = ecs_field(it, Pointcloud, 1);
	for (int i = 0; i < it->count; ++i, p++)
	{
		assert(p->cap > 0);
		assert(p->cap < MAX_PARTICLES);
		p->pos = ecs_os_calloc(p->cap * sizeof(float) * 3);
		p->vel = ecs_os_calloc(p->cap * sizeof(float) * 3);
		p->col = ecs_os_calloc(p->cap * sizeof(uint32_t));
		assert(p->pos);
		assert(p->vel);
		assert(p->col);
	}
}

void PointcloudsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Pointclouds);

	ECS_COMPONENT_DEFINE(world, Pointcloud);

	ECS_OBSERVER(world, Observer, EcsOnSet, Pointcloud);

	ECS_SYSTEM(world, Bouncer, EcsOnUpdate, Pointcloud);
	ecs_enable(world, Bouncer, false);

	ecs_set_hooks(world, Pointcloud, {
		.ctor = ecs_ctor(Pointcloud),
		.move = ecs_move(Pointcloud),
		.dtor = ecs_dtor(Pointcloud),
	});

	ecs_struct(world, {.entity = ecs_id(Pointcloud),
		.members = {
		{.name = "cap", .type = ecs_id(ecs_i32_t)},
		{.name = "count", .type = ecs_id(ecs_i32_t)},
		{.name = "point_size", .type = ecs_id(ecs_f32_t)},
		{.name = "pos", .type = ecs_id(ecs_uptr_t)},
		{.name = "vel", .type = ecs_id(ecs_uptr_t)},
		{.name = "col", .type = ecs_id(ecs_uptr_t)},
	}});

	ecs_set_hooks(world, Pointcloud, {
		.ctor = ecs_ctor(Pointcloud),
		.move = ecs_move(Pointcloud),
		.dtor = ecs_dtor(Pointcloud),
	});
}