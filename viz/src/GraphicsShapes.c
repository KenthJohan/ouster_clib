#include "viz/GraphicsShapes.h"
#include "viz/Geometries.h"
#include "viz/misc.h"

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <platform/fs.h>
#include <platform/log.h>


ECS_COMPONENT_DECLARE(ShapeBufferImpl);
ECS_COMPONENT_DECLARE(ShapeIndex);


static void Setup(ecs_iter_t *it)
{
	ShapeBuffer *shape = ecs_field(it, ShapeBuffer, 1);
	for (int i = 0; i < it->count; ++i, ++shape)
	{
		ecs_entity_t e = it->entities[i];
		print_entity_from_it(it, i);
		/*
		sshape_vertex_t *vertices = ecs_os_calloc_n(sshape_vertex_t, shape->cap_vertices);
		uint16_t *indices = ecs_os_calloc_n(uint16_t, shape->cap_indices);
		sshape_buffer_t buf = {
			.vertices.buffer = (sshape_range){vertices, (sizeof(sshape_vertex_t) * shape->cap_vertices)},
			.indices.buffer = (sshape_range){indices, (sizeof(uint16_t) * shape->cap_indices)},
		};
		*/
		ecs_set(it->world, it->entities[i], ShapeBufferImpl, {0});
	}
}

static void AddSphere(ecs_iter_t *it)
{
	Sphere *sphere = ecs_field(it, Sphere, 1);
	ShapeBufferImpl *shape = ecs_field(it, ShapeBufferImpl, 2);

	for (int i = 0; i < it->count; ++i, ++sphere)
	{
		print_entity_from_it(it, i);
		/*
		shape->buf = sshape_build_sphere(&shape->buf, &(sshape_sphere_t){
			.radius = sphere->r,
			.slices = sphere->slices,
			.stacks = sphere->stacks,
			.random_colors = true,
			});
		sshape_element_range_t element = sshape_element_range(&shape->buf);
		ecs_set(it->world, it->entities[i], ShapeIndex, {element});
		*/
	}
}

void GraphicsShapesImport(ecs_world_t *world)
{
	ECS_MODULE(world, GraphicsShapes);
	ECS_IMPORT(world, Geometries);

	ECS_COMPONENT_DEFINE(world, ShapeBufferImpl);
	ECS_COMPONENT_DEFINE(world, ShapeIndex);

	/*
	ecs_struct(world, {.entity = ecs_id(ShapeIndex),
		.members = {
			{.name = "base_element", .type = ecs_id(ecs_i32_t)},
			{.name = "num_elements", .type = ecs_id(ecs_i32_t)},
		}});
		*/
	
	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Setup,
		.query.filter.terms =
		{
			{.id = ecs_id(ShapeBuffer), .src.flags = EcsSelf},
			{.id = ecs_id(ShapeBufferImpl), .oper = EcsNot}, // Adds this
		}});

	/*
	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = AddSphere,
		.query.filter.terms =
		{
			{.id = ecs_id(Sphere), .src.flags = EcsSelf},
			{.id = ecs_id(ShapeBufferImpl), .src.trav = EcsIsA, .src.flags = EcsUp},
			{.id = ecs_id(ShapeIndex), .oper = EcsNot}, // Adds this
		}});
		*/
}