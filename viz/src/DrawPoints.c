#include "viz/DrawPoints.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"
#include "viz/Pointclouds.h"
#include "viz/Sg.h"
#include "viz/vs_params.h"
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_debugtext.h>
#include <sokol/sokol_app.h>
#include <sokol/HandmadeMath.h>
#include <sokol/align.h>
#include <platform/log.h>
#include <platform/fs.h>

#define NUM_X (32)
#define NUM_Y (32)
#define NUM_VERTS (NUM_X * NUM_Y)

typedef struct
{
	float x;
	float y;
	float z;
	float w;
	uint32_t color;
} vertex_t;





typedef struct
{
	ecs_i32_t cap;

	sg_buffer ibuf;
	sg_bindings bind;

	float rx, ry;
	float point_size;
	vertex_t *vertices;
} DrawPointsState;

ECS_COMPONENT_DECLARE(DrawPointsDesc);
ECS_COMPONENT_DECLARE(DrawPointsState);



void DrawPointsState_Add(ecs_iter_t *it)
{
	DrawPointsDesc *desc = ecs_field(it, DrawPointsDesc, 1); // Self

	for (int i = 0; i < it->count; ++i, ++desc)
	{
		DrawPointsState *s = ecs_get_mut(it->world, it->entities[i], DrawPointsState);
		memset(s, 0, sizeof(DrawPointsState));
		s->cap = desc->cap;
		s->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
			.size = s->cap * sizeof(vertex_t),
			.usage = SG_USAGE_STREAM,
		});
		s->bind.index_buffer.id = SG_INVALID_ID;
		s->vertices = ecs_os_calloc_n(vertex_t, s->cap);
	}
}

void DrawPointsState_Draw(ecs_iter_t *it)
{
	Pointcloud *cloud = ecs_field(it, Pointcloud, 1);		// self or up
	SgPipeline *pip = ecs_field(it, SgPipeline, 2);		    // up
	DrawPointsState *s = ecs_field(it, DrawPointsState, 3); // up
	CamerasCamera *cam = ecs_field(it, CamerasCamera, 4);	// up
	Window *window = ecs_field(it, Window, 5);				// up

	int self1 = ecs_field_is_self(it, 1);

	for (int i = 0; i < it->count; ++i, cloud += self1)
	{
		if (cloud->count <= 0)
		{
			continue;
		}
		int n = ECS_MIN(cloud->count, s->cap);
		for (int j = 0; j < n; ++j)
		{
			vertex_t v;

			v.x = cloud->pos[j * 3 + 0];
			v.y = cloud->pos[j * 3 + 1];
			v.z = cloud->pos[j * 3 + 2];
			v.w = cloud->point_size;
			// v.color = colors[j % 3];
			v.color = cloud->col[j];
			s->vertices[j] = v;
		}
		sg_range range = {.ptr = s->vertices, .size = (size_t)n * sizeof(vertex_t)};
		sg_update_buffer(s->bind.vertex_buffers[0], &range);
		sg_apply_pipeline(pip->id);
		sg_apply_bindings(&s->bind);

		vs_params_t vs_params;
		vs_params.viewport.X = window->w;
		vs_params.viewport.Y = window->h;
		ecs_os_memcpy_t(&vs_params.mvp, &cam->vp, hmm_mat4);
		int slot = 0;
		sg_apply_uniforms(SG_SHADERSTAGE_VS, slot, &(sg_range){.ptr = &vs_params, .size = sizeof(vs_params_t)});
		sg_draw(0, n, 1);
	}
}

void DrawPointsImport(ecs_world_t *world)
{
	ECS_MODULE(world, DrawPoints);
	ecs_set_name_prefix(world, "DrawPoints");
	ECS_IMPORT(world, Cameras);
	ECS_IMPORT(world, Windows);
	ECS_IMPORT(world, Pointclouds);

	ECS_COMPONENT_DEFINE(world, DrawPointsDesc);
	ECS_COMPONENT_DEFINE(world, DrawPointsState);

	ecs_struct(world, {.entity = ecs_id(DrawPointsDesc),
		.members = {
			{.name = "cap", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = DrawPointsState_Add,
		.query.filter.terms =
			{
				{.id = ecs_id(DrawPointsDesc), .src.flags = EcsSelf},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
				{.id = ecs_id(DrawPointsState), .oper = EcsNot}, // Adds this
			}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = DrawPointsState_Draw,
		.query.filter.terms =
			{
				{.id = ecs_id(Pointcloud)},
				{.id = ecs_id(SgPipeline), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(DrawPointsState), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(CamerasCamera), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(Window), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
			}});
}