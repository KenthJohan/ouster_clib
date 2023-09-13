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
	float x, y;
	uint32_t color;
} vertex_t;

static struct
{
	sg_pass_action pass_action;
	sg_buffer vbuf;
	// per-primitive-type data

	sg_buffer ibuf;
	sg_pipeline pip;
	int num_elements;

	float rx, ry;
	float point_size;

	vertex_t vertices[NUM_VERTS];
} state;

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
void setup_vertex_and_index_data(void)
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

void DrawPoints_frame(ecs_world_t *world)
{
	const float t = (float)(sapp_frame_duration() * 60.0);
	state.rx += 0.3f * t;
	state.ry += 0.2f * t;
	const float w = sapp_widthf();
	const float h = sapp_heightf();
	const vs_params_t vs_params = compute_vsparams(w, h, state.rx, state.ry, state.point_size);

	sg_begin_default_passf(&state.pass_action, w, h);
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&(sg_bindings){.vertex_buffers[0] = state.vbuf});
	sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(vs_params));
	sg_draw(0, state.num_elements, 1);
	sg_end_pass();
	sg_commit();
}

static void incr_point_size(void)
{
	state.point_size += 1.0f;
}

static void decr_point_size(void)
{
	state.point_size -= 1.0f;
	if (state.point_size < 1.0f)
	{
		state.point_size = 1.0f;
	}
}

void DrawPointsImport(ecs_world_t *world)
{
	ECS_MODULE(world, DrawInstances);
	ECS_IMPORT(world, Cameras);
	ECS_IMPORT(world, Windows);
	ECS_IMPORT(world, Pointclouds);
}