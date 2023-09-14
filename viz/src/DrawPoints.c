#include "viz/DrawPoints.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"
#include "viz/Pointclouds.h"
#include "vendor/sokol_gfx.h"
#include "vendor/sokol_debugtext.h"
#include "vendor/sokol_app.h"
#include "vendor/HandmadeMath.h"
#include <platform/log.h>
#include <platform/fs.h>

#define NUM_X (32)
#define NUM_Y (32)
#define NUM_VERTS (NUM_X * NUM_Y)

typedef struct
{
	float x, y, z;
	uint32_t color;
} vertex_t;

#if !defined(SOKOL_SHDC_ALIGN)
#if defined(_MSC_VER)
#define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
#else
#define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
#endif
#endif
#define ATTR_vs_position (0)
#define ATTR_vs_color0 (1)
#define SLOT_vs_params (0)
#pragma pack(push, 1)
SOKOL_SHDC_ALIGN(16)
typedef struct vs_params_t
{
	hmm_mat4 mvp;
	float point_size;
	uint8_t _pad_68[12];
} vs_params_t;
#pragma pack(pop)

typedef struct
{
	ecs_i32_t cap;
	vs_params_t vs_params;
	sg_pass_action pass_action;
	// per-primitive-type data

	sg_buffer ibuf;
	sg_pipeline pip;
	sg_bindings bind;

	float rx, ry;
	float point_size;
	vertex_t *vertices;
} DrawPointsState;

ECS_COMPONENT_DECLARE(DrawPointsDesc);
ECS_COMPONENT_DECLARE(DrawPointsState);

// helper function to compute vertex shader params
vs_params_t compute_vsparams(float disp_w, float disp_h, float rx, float ry, float point_size)
{
	hmm_mat4 proj = HMM_Perspective(60.0f, disp_w / disp_h, 0.01f, 10.0f);
	hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 0.0f, 1.5f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
	hmm_mat4 rxm = HMM_Rotate(rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
	hmm_mat4 rym = HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);
	return (vs_params_t){
		.mvp = HMM_MultiplyMat4(view_proj, model),
		.point_size = point_size,
	};
}

// helper function to fill index data
/*
void setup_vertex_and_index_data(DrawPointsState *state)
{
	// vertices
	{
		const float dx = 1.0f / NUM_X;
		const float dy = 1.0f / NUM_Y;
		const float offset_x = -dx * (NUM_X / 2);
		const float offset_y = -dy * (NUM_Y / 2);
		// red      green       yellow
		const uint32_t colors[3] = {0xFF0000DD, 0xFF00DD00, 0xFF00DDDD};
		int i = 0;
		for (int y = 0; y < NUM_Y; y++)
		{
			for (int x = 0; x < NUM_X; x++, i++)
			{
				assert(i < NUM_VERTS);
				vertex_t *vtx = &state.vertices[i];
				vtx->x = x * dx + offset_x;
				vtx->y = y * dy + offset_y;
				vtx->color = colors[i % 3];
			}
		}
		assert(i == NUM_VERTS);
	}
}
*/

static sg_shader create_shader(char *path_fs, char *path_vs)
{
	platform_log("Creating shaders from files %s %s in ", path_fs, path_vs);
	fs_pwd();
	platform_log("\n");
	sg_shader_desc desc = {0};
	desc.attrs[0].name = "position";
	desc.attrs[1].name = "color0";
	desc.vs.source = fs_readfile(path_vs);
	desc.vs.entry = "main";
	desc.vs.uniform_blocks[0].size = 80;
	desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
	desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
	desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
	desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
	desc.fs.source = fs_readfile(path_fs);
	desc.fs.entry = "main";
	desc.label = "primtypes_shader";
	sg_shader shd = sg_make_shader(&desc);
	return shd;
}

/*
void DrawPoints_init(void)
{
	state.point_size = 4.0f;

	// vertex- and index-buffers
	setup_vertex_and_index_data();
	state.vbuf = sg_make_buffer(&(sg_buffer_desc){
		.data = SG_RANGE(state.vertices),
	});

	// create pipeline state objects for each primitive type
	sg_pipeline_desc pip_desc = {
		.layout = {
			.attrs[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT2,
			.attrs[ATTR_vs_color0].format = SG_VERTEXFORMAT_UBYTE4N,
		},
		.shader = create_shader("../../viz/src/points.fs.glsl", "../../viz/src/points.vs.glsl"),
		.depth = {.write_enabled = true, .compare = SG_COMPAREFUNC_LESS_EQUAL},
		.index_type = SG_INDEXTYPE_NONE,
		.primitive_type = SG_PRIMITIVETYPE_POINTS,
	};
	state.pip = sg_make_pipeline(&pip_desc);

	// the number of elements (vertices or indices) to render
	state.num_elements = NUM_VERTS;

	// pass action for clearing the framebuffer
	// state.pass_action = (sg_pass_action){.colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.2f, 0.4f, 1.0f}}};

	state.pass_action = (sg_pass_action){
		.colors[0].load_action = SG_LOADACTION_DONTCARE,
		.depth.load_action = SG_LOADACTION_DONTCARE,
		.stencil.load_action = SG_LOADACTION_DONTCARE};
}
*/

void DrawPointsState_Add(ecs_iter_t *it)
{
	DrawPointsDesc *desc = ecs_field(it, DrawPointsDesc, 1); // Self

	for (int i = 0; i < it->count; ++i, ++desc)
	{
		DrawPointsState *s = ecs_get_mut(it->world, it->entities[i], DrawPointsState);
		s->cap = desc->cap;
		s->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
			.size = s->cap * sizeof(vertex_t),
			.usage = SG_USAGE_STREAM,
		});
		s->bind.index_buffer.id = SG_INVALID_ID;
		sg_pipeline_desc pip_desc = {
			.layout = {
				.attrs[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
				.attrs[ATTR_vs_color0].format = SG_VERTEXFORMAT_UBYTE4N,
			},
			.shader = create_shader("../../viz/src/points.fs.glsl", "../../viz/src/points.vs.glsl"),
			.depth = {.write_enabled = true, .compare = SG_COMPAREFUNC_LESS_EQUAL},
			.index_type = SG_INDEXTYPE_NONE,
			.primitive_type = SG_PRIMITIVETYPE_POINTS,
		};
		s->pip = sg_make_pipeline(&pip_desc);
		s->pass_action = (sg_pass_action){
			.colors[0].load_action = SG_LOADACTION_DONTCARE,
			.depth.load_action = SG_LOADACTION_DONTCARE,
			.stencil.load_action = SG_LOADACTION_DONTCARE};
		s->vertices = ecs_os_calloc_n(vertex_t, s->cap);
	}
}

void DrawPointsState_Draw(ecs_iter_t *it)
{
	Pointcloud *cloud = ecs_field(it, Pointcloud, 1);		// self or up
	DrawPointsState *s = ecs_field(it, DrawPointsState, 2); // up
	Camera *cam = ecs_field(it, Camera, 3);					// up
	Window *window = ecs_field(it, Window, 4);				// up

	int self1 = ecs_field_is_self(it, 1);

	for (int i = 0; i < it->count; ++i, cloud += self1)
	{
		if (cloud->count <= 0)
		{
			continue;
		}
		const uint32_t colors[3] = {0xFF0000DD, 0xFF00DD00, 0xFF00DDDD};
		int n = ECS_MIN(cloud->count, s->cap);
		for (int j = 0; j < n; ++j)
		{
			vertex_t v;

			v.x = cloud->pos[j * 3 + 0];
			v.y = cloud->pos[j * 3 + 1];
			v.z = cloud->pos[j * 3 + 2];
			// v.color = colors[j % 3];
			v.color = cloud->col[j];
			s->vertices[j] = v;
		}
		sg_range range = {.ptr = s->vertices, .size = (size_t)n * sizeof(vertex_t)};
		sg_update_buffer(s->bind.vertex_buffers[0], &range);
		sg_begin_default_passf(&s->pass_action, window->w, window->h);
		sg_apply_pipeline(s->pip);
		sg_apply_bindings(&s->bind);
		s->vs_params.point_size = 4.0f;
		ecs_os_memcpy_t(&s->vs_params.mvp, cam->mvp, hmm_mat4);
		sg_range a = {&s->vs_params, sizeof(vs_params_t)};
		sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &a);
		sg_draw(0, n, 1);
		sg_end_pass();
		sg_commit();
	}
}

void DrawPointsImport(ecs_world_t *world)
{
	ECS_MODULE(world, DrawPoints);
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
									   {.id = ecs_id(DrawPointsState), .src.trav = EcsIsA, .src.flags = EcsUp},
									   {.id = ecs_id(Camera), .src.trav = EcsIsA, .src.flags = EcsUp},
									   {.id = ecs_id(Window), .src.trav = EcsIsA, .src.flags = EcsUp},
									   {.id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
								   }});
}