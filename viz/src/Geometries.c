#include "viz/Geometries.h"
#include <assert.h>

ECS_COMPONENT_DECLARE(ShapeBuffer);
ECS_COMPONENT_DECLARE(Sphere);
ECS_COMPONENT_DECLARE(Position2);
ECS_COMPONENT_DECLARE(Position3);
ECS_COMPONENT_DECLARE(GeometriesInstances);



void Add(ecs_iter_t *it)
{
	GeometriesInstances *ins = ecs_field(it, GeometriesInstances, 1); // up
	Position3 *pos = ecs_field(it, Position3, 1); // self

	//Position3 * pos1 = ecs_vec_grow_t(NULL, &ins->pos, Position3, it->count);

	for (int i = 0; i < it->count; ++i)
	{
		//pos1[i] = pos[i];
	}
}


void GeometriesImport(ecs_world_t *world)
{
	ECS_MODULE(world, Geometries);
	ECS_COMPONENT_DEFINE(world, ShapeBuffer);
	ECS_COMPONENT_DEFINE(world, Sphere);
	ECS_COMPONENT_DEFINE(world, Position2);
	ECS_COMPONENT_DEFINE(world, Position3);
	ECS_COMPONENT_DEFINE(world, GeometriesInstances);

	ecs_struct(world, {.entity = ecs_id(ShapeBuffer),
					   .members = {
						   {.name = "cap_vertices", .type = ecs_id(ecs_i32_t)},
						   {.name = "cap_indices", .type = ecs_id(ecs_i32_t)},
					   }});

	ecs_struct(world, {.entity = ecs_id(Sphere),
					   .members = {
						   {.name = "r", .type = ecs_id(ecs_f32_t)},
						   {.name = "slices", .type = ecs_id(ecs_i32_t)},
						   {.name = "stacks", .type = ecs_id(ecs_i32_t)},
					   }});

	ecs_struct(world, {.entity = ecs_id(Position2),
					   .members = {
						   {.name = "x", .type = ecs_id(ecs_f32_t)},
						   {.name = "y", .type = ecs_id(ecs_f32_t)},
					   }});

	ecs_struct(world, {.entity = ecs_id(Position3),
					   .members = {
						   {.name = "x", .type = ecs_id(ecs_f32_t)},
						   {.name = "y", .type = ecs_id(ecs_f32_t)},
						   {.name = "z", .type = ecs_id(ecs_f32_t)},
					   }});

	/**
	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Add,
		.query.filter.terms =
			{
				{.id = ecs_id(GeometriesInstances), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(Position3), .src.flags = EcsSelf},
			}});
			*/

}