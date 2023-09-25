//------------------------------------------------------------------------------
//  shapes-sapp.c
//  Simple sokol_shape.h demo.
//------------------------------------------------------------------------------
#include "viz/DrawShapes.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"
#include "viz/Pointclouds.h"
#include "viz/Sg.h"
#include "viz/vs_params.h"
#include "viz/GraphicsShapes.h"

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_gl.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_glue.h>
#include <sokol/HandmadeMath.h>
#include <sokol/sokol_shape.h>
#include <sokol/align.h>


#include <easymath/mathtypes.h>
#include <easymath/lin_f32.h>
#include <easymath/quat_f32.h>
#include <easymath/transform.h>

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <platform/fs.h>
#include <platform/log.h>



typedef struct
{
	ecs_i32_t cap;
	sg_bindings bind;
} DrawShapesState;


ECS_COMPONENT_DECLARE(DrawShapesDesc);
ECS_COMPONENT_DECLARE(DrawShapesState);



void DrawShapesState_Add(ecs_iter_t *it)
{
	DrawShapesDesc *desc = ecs_field(it, DrawShapesDesc, 1); // Self
	ShapeBufferImpl *sbuf = ecs_field(it, ShapeBufferImpl, 2);// Up
	for (int i = 0; i < it->count; ++i, ++desc)
	{
		DrawShapesState *s = ecs_get_mut(it->world, it->entities[i], DrawShapesState);
		memset(s, 0, sizeof(DrawShapesState));
		/*
		s->cap = desc->cap;
		assert(sbuf->buf.valid);
		const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&sbuf->buf);
		const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&sbuf->buf);
		s->bind.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);
		s->bind.index_buffer = sg_make_buffer(&ibuf_desc);
		*/
	}
}

void DrawShapesState_Draw(ecs_iter_t *it)
{
	Position3 *pos = ecs_field(it, Position3, 1);
	SgPipeline *pip = ecs_field(it, SgPipeline, 2);  // up
	DrawShapesState *rend = ecs_field(it, DrawShapesState, 3);
	CamerasCamera *cam = ecs_field(it, CamerasCamera, 4);
	ShapeIndex *shape = ecs_field(it, ShapeIndex, 5);

	for (int i = 0; i < it->count; ++i, ++pos)
	{
		sg_apply_pipeline(pip->id);
		sg_apply_bindings(&rend->bind);

		m4f32 t = {M4_IDENTITY};
		m4f32_translation3(&t, &(v3f32){{pos->x, pos->y, pos->z}});
		m4f32 mvp;
		m4f32_mul(&mvp, &cam->vp, &t);

		vs_params_t vs_params = {0};
		ecs_os_memcpy_t(&vs_params.mvp, &mvp, hmm_mat4);
		int slot = 0;
		sg_apply_uniforms(SG_SHADERSTAGE_VS, slot, &(sg_range){.ptr = &vs_params, .size = sizeof(vs_params_t)});
		sg_draw(shape->element.base_element, shape->element.num_elements, 1);
	}
}

void DrawShapesImport(ecs_world_t *world)
{
	ECS_MODULE(world, DrawShapes);
	ecs_set_name_prefix(world, "DrawShapes");
	ECS_IMPORT(world, Cameras);
	ECS_IMPORT(world, Windows);
	ECS_IMPORT(world, Pointclouds);

	ECS_COMPONENT_DEFINE(world, DrawShapesDesc);
	ECS_COMPONENT_DEFINE(world, DrawShapesState);

	ecs_struct(world, {.entity = ecs_id(DrawShapesDesc),
		.members = {
			{.name = "cap", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = DrawShapesState_Add,
		.query.filter.terms =
			{
				{.id = ecs_id(DrawShapesDesc), .src.flags = EcsSelf},
				{.id = ecs_id(ShapeBufferImpl), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
				{.id = ecs_id(DrawShapesState), .oper = EcsNot}, // Adds this
			}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = DrawShapesState_Draw,
		.query.filter.terms =
			{
				{.id = ecs_id(Position3)},
				{.id = ecs_id(SgPipeline), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(DrawShapesState), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(CamerasCamera), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(ShapeIndex), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
			}});
}