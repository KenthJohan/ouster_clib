#include "viz/DrawInstances.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"
#include "viz/Pointclouds.h"
#include "viz/Sg.h"
#include "GraphicsShapes.h"

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_gl.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_glue.h>
#include <sokol/HandmadeMath.h>
#include <sokol/sokol_shape.h>

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <platform/fs.h>
#include <platform/log.h>

#if !defined(SOKOL_SHDC_ALIGN)
#if defined(_MSC_VER)
#define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
#else
#define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
#endif
#endif
#define ATTR_vs_position (0)
#define ATTR_vs_normal (1)
#define ATTR_vs_texcoord (2)
#define ATTR_vs_color0 (3)
#define SLOT_vs_params (0)
#pragma pack(push, 1)
SOKOL_SHDC_ALIGN(16)
typedef struct vs_params_t
{
	hmm_mat4 mvp;
	float draw_mode;
	uint8_t _pad_68[12];
} vs_params_t;
#pragma pack(pop)

enum
{
	BOX = 0,
	PLANE,
	SPHERE,
	CYLINDER,
	TORUS,
	NUM_SHAPES
};

typedef struct
{
	hmm_vec3 pos;
	sshape_element_range_t draw;
} shape_t;

typedef struct
{
	ecs_i32_t cap;
	vs_params_t vs_params;
	sg_pass_action pass_action;
	sg_pipeline pip;
	sg_bindings bind;
} DrawInstancesState;

ECS_COMPONENT_DECLARE(DrawInstancesDesc);
ECS_COMPONENT_DECLARE(DrawInstancesState);

static sg_shader create_shader(char *path_fs, char *path_vs)
{
	platform_log("Creating shaders from files %s %s in ", path_fs, path_vs);
	fs_pwd();
	platform_log("\n");
	sg_shader_desc desc = {0};
	desc.attrs[0].name = "position";
	desc.attrs[1].name = "normal";
	desc.attrs[2].name = "texcoord";
	desc.attrs[3].name = "color0";
	desc.attrs[4].name = "inst_pos";

	desc.vs.source = fs_readfile(path_vs);
	desc.vs.entry = "main";
	desc.vs.uniform_blocks[0].size = sizeof(float) * 5 * 4;
	desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
	desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
	desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
	desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
	desc.fs.source = fs_readfile(path_fs);
	desc.fs.entry = "main";
	desc.label = "instancing_shader";
	sg_shader shd = sg_make_shader(&desc);
	return shd;
}

void DrawInstancesState_Add(ecs_iter_t *it)
{
	DrawInstancesDesc *desc = ecs_field(it, DrawInstancesDesc, 1); // Self
	ShapeBufferImpl *sbuf = ecs_field(it, ShapeBufferImpl, 2);	   // Up

	for (int i = 0; i < it->count; ++i, ++desc)
	{
		DrawInstancesState *rend = ecs_get_mut(it->world, it->entities[i], DrawInstancesState);
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


		
		
		sg_shader shd = create_shader("../../viz/src/instance.fs.glsl", "../../viz/src/instance.vs.glsl");
		// a pipeline object
		rend->pip = sg_make_pipeline(&(sg_pipeline_desc){
			.layout = {
				.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE,
				.buffers[0] = sshape_vertex_buffer_layout_state(),
				.attrs = {
					[0] = sshape_position_vertex_attr_state(),
					[1] = sshape_normal_vertex_attr_state(),
					[2] = sshape_texcoord_vertex_attr_state(),
					[3] = sshape_color_vertex_attr_state(),
					[4] = {.format = SG_VERTEXFORMAT_FLOAT3, .buffer_index = 1}}},
			.shader = shd,
			.index_type = SG_INDEXTYPE_UINT16,
			.cull_mode = SG_CULLMODE_BACK,
			.depth = {
				.compare = SG_COMPAREFUNC_LESS_EQUAL,
				.write_enabled = true,
			},
			.label = "instancing-pipeline"});
			
	}
}

void RenderPointcloud_Draw(ecs_iter_t *it)
{
	Position3 *pos = ecs_field(it, Position3, 1);
	DrawInstancesState *rend = ecs_field(it, DrawInstancesState, 2);
	Camera *cam = ecs_field(it, Camera, 3);
	ShapeIndex *shape = ecs_field(it, ShapeIndex, 4);
	Window *window = ecs_field(it, Window, 5); // up

	for (int i = 0; i < it->count; ++i, ++pos)
	{
		//printf("%s\n", ecs_get_name(it->world, it->entities[i]));
		if (rend->cap <= 0)
		{
			continue;
		}

		// update instance data
		//sg_range range = {.ptr = pos, .size = (size_t)1 * sizeof(hmm_vec3)};
		//sg_update_buffer(rend->bind.vertex_buffers[1], &range);

		int offset = sg_append_buffer(rend->bind.vertex_buffers[1], &(sg_range) { .ptr=pos, .size=(size_t)1 * sizeof(hmm_vec3) });
		rend->bind.vertex_buffer_offsets[1] = offset;
		
		sg_apply_pipeline(rend->pip);
		sg_apply_bindings(&rend->bind);

		ecs_os_memcpy_t(&rend->vs_params.mvp, cam->mvp, hmm_mat4);
		sg_range a = {&rend->vs_params, sizeof(vs_params_t)};
		sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &a);
		sg_draw(shape->element.base_element, shape->element.num_elements, 1);

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

	ecs_struct(world, {.entity = ecs_id(DrawInstancesDesc),
					   .members = {
						   {.name = "cap", .type = ecs_id(ecs_i32_t)},
					   }});

	ecs_struct(world, {.entity = ecs_id(DrawInstancesState),
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

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = RenderPointcloud_Draw,
		.query.filter.terms =
			{
				{.id = ecs_id(Position3)},
				{.id = ecs_id(DrawInstancesState), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(Camera), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(ShapeIndex), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(Window), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
			}});
			
}