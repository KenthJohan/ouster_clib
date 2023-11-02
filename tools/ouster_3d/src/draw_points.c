
#include "draw_points.h"

#include "sokol_app.h"

#include "HandmadeMath.h"
#include <stdlib.h>
#include <ouster_clib.h>
#include <string.h>

typedef struct {
	float mvp[4*4];
	float extra[4];
} vs_params_t;


void draw_points_pass(draw_points_t *app, m4f32 *vp)
{
	sg_apply_pipeline(app->pip);
	sg_apply_bindings(&(sg_bindings){
	    .vertex_buffers[0] = app->vbuf,
	    .index_buffer.id = SG_INVALID_ID,
	});
	sg_update_buffer(app->vbuf, &(sg_range){.ptr = app->vertices, .size = app->vertices_count*sizeof(vertex_t)});
	const float w = sapp_widthf();
	const float h = sapp_heightf();
	vs_params_t vs_params = {0};
	vs_params.extra[0] = w;
	vs_params.extra[1] = h;
	//vs_params.point_size = app->point_size;
	memcpy(vs_params.mvp, vp, sizeof(m4f32));
	sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(vs_params));
	sg_draw(0, app->vertices_count, 1);
}

// helper function to fill index data
void setup_vertex_and_index_data(draw_points_t *app)
{
	vertex_t *v = app->vertices;
	int n = app->vertices_count;
	for(int i = 0; i < n; ++i, ++v)
	{
		v->x = ((float)rand() / (float)RAND_MAX) * sqrtf((float)n) - 50.0f;
		v->y = ((float)rand() / (float)RAND_MAX) * sqrtf((float)n) - 50.0f;
		v->z = ((float)rand() / (float)RAND_MAX) * sqrtf((float)n) - 50.0f;
		v->w = 5.0f;
		v->color = rand();
	}
}

void draw_points_init(draw_points_t *app)
{
	app->point_size = 10.0f;
	app->vertices_cap = 1000 * 1000;

	app->vertices = calloc(1, sizeof(vertex_t) * app->vertices_cap);
	app->vertices_count = app->vertices_cap;

	setup_vertex_and_index_data(app);
	app->vbuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(vertex_t) * app->vertices_cap,
		.usage = SG_USAGE_STREAM
	});
	//sg_update_buffer(app->vbuf, &SG_RANGE(app->vertices));


	char const *vs = ouster_fs_readfile("shader2.vs.glsl");
	char const *fs = ouster_fs_readfile("shader2.fs.glsl");
	sg_shader_desc desc = {0};
	desc.attrs[0].name = "position";
	desc.attrs[1].name = "color0";
	desc.vs.source = vs;
	desc.vs.entry = "main";
	desc.vs.uniform_blocks[0].size = 80;
	desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
	desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
	desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
	desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
	desc.fs.source = fs;
	desc.fs.entry = "main";
	desc.label = "primtypes_shader";
	sg_shader shd = sg_make_shader(&desc);

	// create pipeline state objects for each primitive type
	sg_pipeline_desc pip_desc = {
	    .layout = {
	        .attrs[0].format = SG_VERTEXFORMAT_FLOAT4,
	        .attrs[1].format = SG_VERTEXFORMAT_UBYTE4N,
	    },
	    .shader = shd,
	    .depth = {.write_enabled = true, .compare = SG_COMPAREFUNC_LESS_EQUAL}};

	pip_desc.index_type = SG_INDEXTYPE_NONE;
	pip_desc.primitive_type = SG_PRIMITIVETYPE_POINTS;
	app->pip = sg_make_pipeline(&pip_desc);
}