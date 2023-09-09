//------------------------------------------------------------------------------
//  shapes-sapp.c
//  Simple sokol_shape.h demo.
//------------------------------------------------------------------------------
#include "viz/DrawShapes.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"

#include "vendor/sokol_app.h"
#include "vendor/sokol_gfx.h"
#include "vendor/sokol_log.h"
#include "vendor/sokol_glue.h"
#include "vendor/sokol_shape.h"
#include "vendor/sokol_debugtext.h"
#include "vendor/HandmadeMath.h"
#include "vendor//dbgui.h"

#include <platform/fs.h>
#include <platform/log.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>



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
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    hmm_mat4 mvp;
    float draw_mode;
    uint8_t _pad_68[12];
} vs_params_t;
#pragma pack(pop)

typedef struct {
    hmm_vec3 pos;
    sshape_element_range_t draw;
} shape_t;

enum {
    BOX = 0,
    PLANE,
    SPHERE,
    CYLINDER,
    TORUS,
    NUM_SHAPES
};

static struct {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_buffer vbuf;
    sg_buffer ibuf;
    shape_t shapes[NUM_SHAPES];
    vs_params_t vs_params;
    float rx, ry;
} state;

/*
static inline const sg_shader_desc* shapes_shader_desc(sg_backend backend) {
  if (backend == SG_BACKEND_GLCORE33) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "position";
      desc.attrs[1].name = "normal";
      desc.attrs[2].name = "texcoord";
      desc.attrs[3].name = "color0";
      desc.vs.source = vs_source_glsl330;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 80;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
      desc.fs.source = fs_source_glsl330;
      desc.fs.entry = "main";
      desc.label = "shapes_shader";
    }
    return &desc;
  }
  return 0;
}
*/

static sg_shader create_shader(char * path_fs, char * path_vs)
{
    platform_log("Creating shaders from files %s %s in ", path_fs, path_vs);
    fs_pwd();
    platform_log("\n");
    sg_shader_desc desc = {0};
    desc.attrs[0].name = "position";
    desc.attrs[1].name = "normal";
    desc.attrs[2].name = "texcoord";
    desc.attrs[3].name = "color0";
    desc.vs.source = fs_readfile(path_vs);
    desc.vs.entry = "main";
    desc.vs.uniform_blocks[0].size = 80;
    desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
    desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
    desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
    desc.fs.source = fs_readfile(path_fs);
    desc.fs.entry = "main";
    desc.label = "shapes_shader";
    sg_shader shd = sg_make_shader(&desc);
    return shd;
}



void draw_shapes_init(void)
{
    // clear to black
    /*
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f } }
    };
    */
    state.pass_action = (sg_pass_action) {
        .colors[0].load_action = SG_LOADACTION_DONTCARE ,
        .depth.load_action = SG_LOADACTION_DONTCARE,
        .stencil.load_action = SG_LOADACTION_DONTCARE
    };

    // shader and pipeline object
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = create_shader("../../viz/src/shapes.fs.glsl", "../../viz/src/shapes.vs.glsl"),
        .layout = {
            .buffers[0] = sshape_vertex_buffer_layout_state(),
            .attrs = {
                [0] = sshape_position_vertex_attr_state(),
                [1] = sshape_normal_vertex_attr_state(),
                [2] = sshape_texcoord_vertex_attr_state(),
                [3] = sshape_color_vertex_attr_state()
            }
        },
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_NONE,
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    // shape positions
    state.shapes[BOX].pos = HMM_Vec3(-1.0f, 1.0f, 0.0f);
    state.shapes[PLANE].pos = HMM_Vec3(1.0f, 1.0f, 0.0f);
    state.shapes[SPHERE].pos = HMM_Vec3(-2.0f, -1.0f, 0.0f);
    state.shapes[CYLINDER].pos = HMM_Vec3(2.0f, -1.0f, 0.0f);
    state.shapes[TORUS].pos = HMM_Vec3(0.0f, -1.0f, 0.0f);

    // generate shape geometries
    sshape_vertex_t vertices[6 * 1024];
    uint16_t indices[16 * 1024];
    sshape_buffer_t buf = {
        .vertices.buffer = SSHAPE_RANGE(vertices),
        .indices.buffer  = SSHAPE_RANGE(indices),
    };
    buf = sshape_build_box(&buf, &(sshape_box_t){
        .width  = 1.0f,
        .height = 1.0f,
        .depth  = 1.0f,
        .tiles  = 10,
        .random_colors = true,
    });
    state.shapes[BOX].draw = sshape_element_range(&buf);
    buf = sshape_build_plane(&buf, &(sshape_plane_t){
        .width = 1.0f,
        .depth = 1.0f,
        .tiles = 10,
        .random_colors = true,
    });
    state.shapes[PLANE].draw = sshape_element_range(&buf);
    buf = sshape_build_sphere(&buf, &(sshape_sphere_t) {
        .radius = 0.75f,
        .slices = 36,
        .stacks = 20,
        .random_colors = true,
    });
    state.shapes[SPHERE].draw = sshape_element_range(&buf);
    buf = sshape_build_cylinder(&buf, &(sshape_cylinder_t) {
        .radius = 0.5f,
        .height = 1.5f,
        .slices = 36,
        .stacks = 10,
        .random_colors = true,
    });
    state.shapes[CYLINDER].draw = sshape_element_range(&buf);
    buf = sshape_build_torus(&buf, &(sshape_torus_t) {
        .radius = 0.5f,
        .ring_radius = 0.3f,
        .rings = 36,
        .sides = 18,
        .random_colors = true,
    });
    state.shapes[TORUS].draw = sshape_element_range(&buf);
    assert(buf.valid);

    // one vertex/index-buffer-pair for all shapes
    const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
    const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
    state.vbuf = sg_make_buffer(&vbuf_desc);
    state.ibuf = sg_make_buffer(&ibuf_desc);
}


hmm_mat4 proj1(float dt)
{
    const float t = (float)(dt * 60.0);
    state.rx += 1.0f * t;
    state.ry += 2.0f * t;
    hmm_mat4 proj = HMM_Perspective(60.0f, sapp_widthf()/sapp_heightf(), 0.01f, 10.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
    hmm_mat4 rxm = HMM_Rotate(state.rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
    hmm_mat4 rym = HMM_Rotate(state.ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);
    return HMM_MultiplyMat4(view_proj, model);
}

hmm_mat4 proj2(float dt)
{
    // view-projection matrix...
    hmm_mat4 proj = HMM_Perspective(60.0f, sapp_widthf()/sapp_heightf(), 0.01f, 10.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
    return view_proj;
}

hmm_mat4 model1(float dt)
{
    // model-rotation matrix
    const float t = (float)(dt * 60.0);
    state.rx += 1.0f * t;
    state.ry += 2.0f * t;
    hmm_mat4 rxm = HMM_Rotate(state.rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
    hmm_mat4 rym = HMM_Rotate(state.ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 rm = HMM_MultiplyMat4(rxm, rym);
    return rm;
}



void draw_shapes_frame(ecs_world_t * world)
{
    // help text
    sdtx_canvas(sapp_width()*0.5f, sapp_height()*0.5f);
    sdtx_pos(0.5f, 0.5f);
    sdtx_puts("press key to switch draw mode:\n\n"
              "  1: vertex normals\n"
              "  2: texture coords\n"
              "  3: vertex color");



    float dt = sapp_frame_duration();
    hmm_mat4 view_proj = proj2(dt);
    hmm_mat4 rm = model1(dt);

    ecs_entity_t e = ecs_lookup(world, "camera1");

    Camera const * cam = ecs_get(world, e, Camera);
    ecs_os_memcpy_t(&view_proj, cam->mvp, hmm_mat4);


    // render shapes...
    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&(sg_bindings) {
        .vertex_buffers[0] = state.vbuf,
        .index_buffer = state.ibuf
    });
    for (int i = 0; i < NUM_SHAPES; i++) {
        // per shape model-view-projection matrix
        hmm_mat4 model = HMM_MultiplyMat4(HMM_Translate(state.shapes[i].pos), rm);
        state.vs_params.mvp = HMM_MultiplyMat4(view_proj, model);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(state.vs_params));
        sg_draw(state.shapes[i].draw.base_element, state.shapes[i].draw.num_elements, 1);
    }
    sdtx_draw();
    __dbgui_draw();
    sg_end_pass();
    sg_commit();
}

