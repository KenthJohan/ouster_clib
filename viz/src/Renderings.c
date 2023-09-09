#include "viz/Renderings.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"

#include "vendor/sokol_gfx.h"
#include "vendor/sokol_gl.h"
#include "vendor/sokol_log.h"
#include "vendor/sokol_glue.h"
#include "vendor/HandmadeMath.h"
#include "vendor/sokol_shape.h"

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <platform/fs.h>
#include <platform/log.h>


#define MAX_PARTICLES (512 * 1024)

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


#define MAX_PARTICLES (512 * 1024)
#define NUM_PARTICLES_EMITTED_PER_FRAME (10)

enum {
    BOX = 0,
    PLANE,
    SPHERE,
    CYLINDER,
    TORUS,
    NUM_SHAPES
};

typedef struct {
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
    shape_t shapes[NUM_SHAPES];
    int cur_num_particles;
    float ry;
    hmm_vec3 * pos;
    hmm_vec3 * vel;
} RenderPointcloud;



ECS_COMPONENT_DECLARE(RenderingsDraw);
ECS_COMPONENT_DECLARE(RenderPointcloud);


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


void RenderPointcloud_Setup(ecs_iter_t *it)
{
    RenderPointcloud *rend = ecs_field(it, RenderPointcloud, 1);
    assert(rend->pos ==  NULL);
    assert(rend->pos ==  NULL);
    for(int i = 0; i < it->count; ++i, ++rend)
    {
        /*
        rend->pass_action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0f, 0.0f, 0.0f, 0.0f }
            }
        };
        */

        rend->pass_action  = (sg_pass_action) {
            .colors[0].load_action = SG_LOADACTION_DONTCARE ,
            .depth.load_action = SG_LOADACTION_DONTCARE,
            .stencil.load_action = SG_LOADACTION_DONTCARE
        };

        sshape_vertex_t vertices[6 * 1024];
        uint16_t indices[16 * 1024];
        sshape_buffer_t buf = {
            .vertices.buffer = SSHAPE_RANGE(vertices),
            .indices.buffer  = SSHAPE_RANGE(indices),
        };
        
        buf = sshape_build_sphere(&buf, &(sshape_sphere_t) {
            .radius = 0.01f,
            .slices = 10,
            .stacks = 8,
            .random_colors = true,
        });
        
        /*
        buf = sshape_build_box(&buf, &(sshape_box_t){
            .width  = 1.0f,
            .height = 1.0f,
            .depth  = 1.0f,
            .tiles  = 10,
            .random_colors = true,
        });
        */
        rend->shapes[BOX].draw = sshape_element_range(&buf);
        assert(buf.valid);

        const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
        const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
        rend->bind.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);
        rend->bind.index_buffer = sg_make_buffer(&ibuf_desc);
        rend->bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc){
            .size = MAX_PARTICLES * sizeof(float) * 3,
            .usage = SG_USAGE_STREAM,
            .label = "instance-data"
        });
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
                    [4] = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=1 }
                }
            },
            .shader = shd,
            .index_type = SG_INDEXTYPE_UINT16,
            .cull_mode = SG_CULLMODE_BACK,
            .depth = {
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "instancing-pipeline"
        });



        //Temporary:
        rend->cap = MAX_PARTICLES;
        rend->pos = ecs_os_calloc_n(hmm_vec3, rend->cap);
        rend->vel = ecs_os_calloc_n(hmm_vec3, rend->cap);

        ecs_set(it->world, it->entities[i], RenderingsDraw, {0});
    }
}



void update(hmm_vec3 * pos, hmm_vec3 * vel, int * last, int cap, int emit_per_frame, float t)
{
    // emit new particles
    for (int i = 0; i < emit_per_frame; i++) {
        if ((*last) < cap) {
            pos[(*last)] = HMM_Vec3(0.0, 0.0, 0.0);
            vel[(*last)] = HMM_Vec3(
                ((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f,
                ((float)(rand() & 0x7FFF) / 0x7FFF) * 0.5f + 2.0f,
                ((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f);
            (*last)++;
        } else {
            break;
        }
    }

    // update particle positions
    for (int i = 0; i < (*last); i++) {
        vel[i].Y -= 1.0f * t;
        pos[i].X += vel[i].X * t;
        pos[i].Y += vel[i].Y * t;
        pos[i].Z += vel[i].Z * t;
        // bounce back from 'ground'
        if (pos[i].Y < -2.0f) {
            pos[i].Y = -1.8f;
            vel[i].Y = -vel[i].Y;
            vel[i].X *= 0.8f; vel[i].Y *= 0.8f; vel[i].Z *= 0.8f;
        }
    }
}


void RenderPointcloud_Draw(ecs_iter_t *it)
{
    RenderPointcloud *rend = ecs_field(it, RenderPointcloud, 1);
    Camera *cam = ecs_field(it, Camera, 2);
    Window *window = ecs_field(it, Window, 3);
    
    for(int i = 0; i < it->count; ++i, ++rend, ++cam)
    {
        //printf("%s\n", ecs_get_name(it->world, it->entities[i]));
        if(rend->cap <= 0){return;}

        update(rend->pos, rend->vel, &rend->cur_num_particles, rend->cap, NUM_PARTICLES_EMITTED_PER_FRAME, window->dt);

        // update instance data
        sg_update_buffer(rend->bind.vertex_buffers[1], &(sg_range){
            .ptr = rend->pos,
            .size = (size_t)rend->cur_num_particles * sizeof(hmm_vec3)
        });

        sg_begin_default_pass(&rend->pass_action, window->w, window->h);
        sg_apply_pipeline(rend->pip);
        sg_apply_bindings(&rend->bind);

        ecs_os_memcpy_t(&rend->vs_params.mvp, cam->mvp, hmm_mat4);
        sg_range a = { &rend->vs_params, sizeof(vs_params_t) };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &a);
        sg_draw(rend->shapes[BOX].draw.base_element, rend->shapes[i].draw.num_elements, rend->cur_num_particles);
        sg_end_pass();
    }
}











void Pointcloud_OnSet(ecs_iter_t *it)
{
    Pointcloud *state = ecs_field(it, Pointcloud, 1);
    for(int i = 0; i < it->count; ++i, ++state)
    {
    }
}


void Pointcloud_Draw(ecs_iter_t *it)
{
    Pointcloud *potincloud = ecs_field(it, Pointcloud, 1);
    RenderPointcloud *render = ecs_field(it, RenderPointcloud, 2);
    for(int i = 0; i < it->count; ++i)
    {

    }
}


void RenderingsImport(ecs_world_t *world)
{
    ECS_MODULE(world, Renderings);
    ECS_IMPORT(world, Cameras);
    ECS_IMPORT(world, Geometries);
    ECS_IMPORT(world, Windows);
    

    ECS_COMPONENT_DEFINE(world, RenderingsDraw);
    ECS_COMPONENT_DEFINE(world, RenderPointcloud);


    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = RenderPointcloud_Setup,
        .query.filter.terms =
        {
            { .id = ecs_id(RenderPointcloud) },
            { .id = ecs_id(Camera) },
            { .id = ecs_id(RenderingsDraw), .oper=EcsNot },
            { .id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext) }
            
            //{ .id = ecs_id(RenderPointcloud), .src.trav = EcsIsA, .src.flags = EcsUp },
            //{ .id = ecs_id(Camera), .src.trav = EcsIsA, .src.flags = EcsUp },
        }
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = RenderPointcloud_Draw,
        .query.filter.terms =
        {
            { .id = ecs_id(RenderPointcloud) },
            { .id = ecs_id(Camera) },
            { .id = ecs_id(Window) },
            { .id = ecs_id(RenderingsDraw) },
            { .id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext) }
            
            //{ .id = ecs_id(RenderPointcloud), .src.trav = EcsIsA, .src.flags = EcsUp },
            //{ .id = ecs_id(Camera), .src.trav = EcsIsA, .src.flags = EcsUp },
        }
    });

	ecs_struct(world, {
	.entity = ecs_id(RenderPointcloud),
	.members = {
	{ .name = "cap", .type = ecs_id(ecs_i32_t) },
	}
	});


    //ECS_SYSTEM(world, Pointcloud_OnSet, EcsOnUpdate, Pointcloud, !RenderPointcloud(parent));
    //ECS_SYSTEM(world, Pointcloud_Draw, EcsOnUpdate, Pointcloud, RenderPointcloud(parent));
}