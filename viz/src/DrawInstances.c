#include "viz/DrawInstances.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"
#include "viz/Pointclouds.h"
#include "viz/Sg.h"
#include "viz/vs_params.h"
#include "viz/misc.h"
#include "viz/GraphicsShapes.h"

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_gl.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_glue.h>
#include <sokol/HandmadeMath.h>
#include <sokol/sokol_shape.h>
#include <sokol/align.h>

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <platform/fs.h>
#include <platform/log.h>


typedef struct
{
	hmm_vec3 pos;
	sshape_element_range_t draw;
} shape_t;

typedef struct
{
	ecs_i32_t cap;
	sg_bindings bind;
} DrawInstancesState;


typedef struct
{
	char buf[230];
} Buffen230;

ECS_COMPONENT_DECLARE(DrawInstancesDesc);
ECS_COMPONENT_DECLARE(DrawInstancesState);
ECS_COMPONENT_DECLARE(Buffen230);


void DrawInstancesState_Add(ecs_iter_t *it)
{
	DrawInstancesDesc *desc = ecs_field(it, DrawInstancesDesc, 1); // Self
	//ShapeBufferImpl *sbuf = ecs_field(it, ShapeBufferImpl, 2);	   // Up
	for (int i = 0; i < it->count; ++i)
	{
		ecs_entity_t e = it->entities[i];
		print_entity_from_it(it, i);
		DrawInstancesState *rend = ecs_get_mut(it->world, e, DrawInstancesState);
		printf("sizeof(DrawInstancesState):%i\n", (int)sizeof(DrawInstancesState));
		continue;
		/*
		memset(rend, 0, sizeof(DrawInstancesState));
		rend->cap = desc->cap;
		assert(sbuf->buf.valid);
		const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&sbuf->buf);
		const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&sbuf->buf);
		rend->bind.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);
		rend->bind.index_buffer = sg_make_buffer(&ibuf_desc);
		rend->bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc){
			.size = rend->cap * sizeof(float) * 3,
			.usage = SG_USAGE_STREAM,
			.label = "instance-data"});
			*/
	}
}

void RenderPointcloud_Draw(ecs_iter_t *it)
{
	Position3 *pos = ecs_field(it, Position3, 1);
	SgPipeline *pip = ecs_field(it, SgPipeline, 2);  // up
	DrawInstancesState *rend = ecs_field(it, DrawInstancesState, 3);
	CamerasCamera *cam = ecs_field(it, CamerasCamera, 4);
	ShapeIndex *shape = ecs_field(it, ShapeIndex, 5);
	for (int i = 0; i < it->count; ++i, ++pos)
	{
		/*
		if (rend->cap <= 0)
		{
			continue;
		}

		sg_bindings bind = 
		{
			.index_buffer
		}

		int offset = sg_append_buffer(rend->bind.vertex_buffers[1], &(sg_range) { .ptr=pos, .size=(size_t)1 * sizeof(hmm_vec3) });
		rend->bind.vertex_buffer_offsets[1] = offset;
		sg_apply_pipeline(pip->id);
		sg_apply_bindings(&rend->bind);
		vs_params_t vs_params = {0};
		ecs_os_memcpy_t(&vs_params.mvp, &cam->vp, hmm_mat4);
		int slot = 0;
		sg_apply_uniforms(SG_SHADERSTAGE_VS, slot, &(sg_range){&vs_params, sizeof(vs_params_t)});
		sg_draw(shape->element.base_element, shape->element.num_elements, 1);
		*/
	}
}



void DrawInstancesImport(ecs_world_t *world)
{
	ECS_MODULE(world, DrawInstances);
	ecs_set_name_prefix(world, "DrawInstances");
	ECS_IMPORT(world, Cameras);
	ECS_IMPORT(world, Geometries);
	ECS_IMPORT(world, Windows);
	ECS_IMPORT(world, Pointclouds);
	ECS_IMPORT(world, GraphicsShapes);

	ECS_COMPONENT_DEFINE(world, DrawInstancesDesc);
	ECS_COMPONENT_DEFINE(world, DrawInstancesState);
	ECS_COMPONENT_DEFINE(world, Buffen230);

	ecs_struct(world, {.entity = ecs_id(DrawInstancesDesc),
		.members = {
			{.name = "cap", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = DrawInstancesState_Add,
		.query.filter.terms =
			{
				{.id = ecs_id(DrawInstancesDesc), .src.flags = EcsSelf},
				{.id = ecs_id(ShapeBufferImpl), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
				{.id = ecs_id(DrawInstancesState), .oper = EcsNot}, // Adds this
			}});

	/*
	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = RenderPointcloud_Draw,
		.query.filter.terms =
			{
				{.id = ecs_id(Position3)},
				{.id = ecs_id(SgPipeline), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(DrawInstancesState), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(CamerasCamera), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(ShapeIndex), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
			}});
			*/


		ecs_entity_t e = ecs_new_entity(world, "Buffen230");
		Buffen230 *b230 = ecs_get_mut(world, e, Buffen230);
			
}