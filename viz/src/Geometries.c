#include "viz/Geometries.h"
#include <assert.h>

ECS_COMPONENT_DECLARE(Position2);
ECS_COMPONENT_DECLARE(Position3);






void GeometriesImport(ecs_world_t *world)
{
    ECS_MODULE(world, Geometries);
    ECS_COMPONENT_DEFINE(world, Position2);
    ECS_COMPONENT_DEFINE(world, Position3);



	ecs_struct(world, {
	.entity = ecs_id(Position2),
	.members = {
	{ .name = "x", .type = ecs_id(ecs_f32_t) },
	{ .name = "y", .type = ecs_id(ecs_f32_t) },
	}
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