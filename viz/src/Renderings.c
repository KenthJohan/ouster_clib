#include "viz/Geometries.h"
#include "sokol_gfx.h"
#include "sokol_gl.h"
#include "sokol_app.h"
#include <math.h>
#include <stdlib.h>
#include <platform/fs.h>
#include <platform/log.h>
#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "HandmadeMath.h"

#define MAX_PARTICLES (512 * 1024)
#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_inst_pos (2)
#define SLOT_vs_params (0)
#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    hmm_mat4 mvp;
} vs_params_t;
#pragma pack(pop)


#define MAX_PARTICLES (512 * 1024)
#define NUM_PARTICLES_EMITTED_PER_FRAME (10)
typedef struct
{
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
    int cur_num_particles;
    float ry;
    int cap;
    hmm_vec3 * pos;
    hmm_vec3 * vel;
} RenderPointcloud;


ECS_COMPONENT_DECLARE(RenderPointcloud);


sg_shader create_shader(char * path_fs, char * path_vs)
{
    platform_log("Creating shaders from files %s %s in ", path_fs, path_vs);
    fs_pwd();
    platform_log("\n");
    sg_shader_desc desc = {0};
    desc.attrs[0].name = "pos";
    desc.attrs[1].name = "color0";
    desc.attrs[2].name = "inst_pos";
    desc.vs.source = fs_readfile(path_vs);
    desc.vs.entry = "main";
    desc.vs.uniform_blocks[0].size = 64;
    desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
    desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
    desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
    desc.vs.uniform_blocks[0].uniforms[0].array_count = 4;
    desc.fs.source = fs_readfile(path_fs);
    desc.fs.entry = "main";
    desc.label = "instancing_shader";
    sg_shader shd = sg_make_shader(&desc);
    return shd;
}


void RenderPointcloud_OnSet(ecs_iter_t *it)
{
    RenderPointcloud *rend = ecs_field(it, RenderPointcloud, 1);
    for(int i = 0; i < it->count; ++i, ++rend)
    {
        rend->pass_action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f }
            }
        };
        const float r = 0.05f;
        const float vertices[] = {
            // positions            colors
            0.0f,   -r, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
            r, 0.0f, r,          0.0f, 1.0f, 0.0f, 1.0f,
            r, 0.0f, -r,         0.0f, 0.0f, 1.0f, 1.0f,
            -r, 0.0f, -r,         1.0f, 1.0f, 0.0f, 1.0f,
            -r, 0.0f, r,          0.0f, 1.0f, 1.0f, 1.0f,
            0.0f,    r, 0.0f,       1.0f, 0.0f, 1.0f, 1.0f
        };
        rend->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
            .data = SG_RANGE(vertices),
            .label = "geometry-vertices"
        });
        const uint16_t indices[] = {
            0, 1, 2,    0, 2, 3,    0, 3, 4,    0, 4, 1,
            5, 1, 2,    5, 2, 3,    5, 3, 4,    5, 4, 1
        };
        rend->bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .data = SG_RANGE(indices),
            .label = "geometry-indices"
        });
        rend->bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc){
            .size = MAX_PARTICLES * sizeof(float) * 3,
            .usage = SG_USAGE_STREAM,
            .label = "instance-data"
        });
        sg_shader shd = create_shader("../../viz/src/shader.fs.glsl", "../../viz/src/shader.vs.glsl");
        // a pipeline object
        rend->pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                // vertex buffer at slot 1 must step per instance
                .buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE,
                .attrs = {
                    [ATTR_vs_pos]      = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=0 },
                    [ATTR_vs_color0]   = { .format=SG_VERTEXFORMAT_FLOAT4, .buffer_index=0 },
                    [ATTR_vs_inst_pos] = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=1 }
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
        rend->pos = ecs_os_calloc_n(hmm_vec3, rend->cap);
        rend->vel = ecs_os_calloc_n(hmm_vec3, rend->cap);

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
    for(int i = 0; i < it->count; ++i, ++rend)
    {
        const float frame_time = (float)(sapp_frame_duration());

        update(rend->pos, rend->vel, &rend->cur_num_particles, MAX_PARTICLES, NUM_PARTICLES_EMITTED_PER_FRAME, frame_time);

        // update instance data
        sg_update_buffer(rend->bind.vertex_buffers[1], &(sg_range){
            .ptr = rend->pos,
            .size = (size_t)rend->cur_num_particles * sizeof(hmm_vec3)
        });


        // model-view-projection matrix
        hmm_mat4 proj = HMM_Perspective(60.0f, sapp_widthf()/sapp_heightf(), 0.01f, 50.0f);
        hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 12.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
        hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
        rend->ry += 60.0f * frame_time;
        vs_params_t vs_params;
        vs_params.mvp = HMM_MultiplyMat4(view_proj, HMM_Rotate(rend->ry, HMM_Vec3(0.0f, 1.0f, 0.0f)));

        // ...and draw
        sg_begin_default_pass(&rend->pass_action, sapp_width(), sapp_height());
        sg_apply_pipeline(rend->pip);
        sg_apply_bindings(&rend->bind);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
        sg_draw(0, 24, rend->cur_num_particles);
        sg_end_pass();
    }
}










ecs_entity_t EntRenderPointcloud;

void Pointcloud_OnSet(ecs_iter_t *it)
{
    Pointcloud *state = ecs_field(it, Pointcloud, 1);
    for(int i = 0; i < it->count; ++i, ++state)
    {
        ecs_add_pair(it->world, it->entities[i], EcsChildOf, EntRenderPointcloud);
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
    ECS_COMPONENT_DEFINE(world, RenderPointcloud);
    ECS_OBSERVER(world, RenderPointcloud_OnSet, EcsOnSet, RenderPointcloud);
    ECS_SYSTEM(world, RenderPointcloud_Draw, EcsOnUpdate, RenderPointcloud);
    ECS_SYSTEM(world, Pointcloud_OnSet, EcsOnUpdate, Pointcloud, !RenderPointcloud(parent));
    ECS_SYSTEM(world, Pointcloud_Draw, EcsOnUpdate, Pointcloud, RenderPointcloud(parent));

    EntRenderPointcloud = ecs_new_entity(world, "RenderPointcloud");
    ecs_set(world, EntRenderPointcloud, RenderPointcloud, {.cap = MAX_PARTICLES});
}