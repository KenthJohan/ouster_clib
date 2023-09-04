#include "viz/Geometries.h"
#include "sokol_gfx.h"
#include "sokol_gl.h"
#include "sokol_app.h"
#include <math.h>
#include <platform/fs.h>

#define MAX_PARTICLES (512 * 1024)
#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_inst_pos (2)
#define SLOT_vs_params (0)

typedef struct
{
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
} RenderPointcloud;


ECS_COMPONENT_DECLARE(RenderPointcloud);


sg_shader create_shader(char * path_fs, char * path_vs)
{
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
    RenderPointcloud *state = ecs_field(it, RenderPointcloud, 1);
    for(int i = 0; i < it->count; ++i, ++state)
    {
        state->pass_action = (sg_pass_action) {
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
        state->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
            .data = SG_RANGE(vertices),
            .label = "geometry-vertices"
        });
        const uint16_t indices[] = {
            0, 1, 2,    0, 2, 3,    0, 3, 4,    0, 4, 1,
            5, 1, 2,    5, 2, 3,    5, 3, 4,    5, 4, 1
        };
        state->bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .data = SG_RANGE(indices),
            .label = "geometry-indices"
        });
        state->bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc){
            .size = MAX_PARTICLES * sizeof(float) * 3,
            .usage = SG_USAGE_STREAM,
            .label = "instance-data"
        });
        sg_shader shd = create_shader("viz/src/shader.fs.glsl", "viz/src/shader.vs.glsl");
        // a pipeline object
        state->pip = sg_make_pipeline(&(sg_pipeline_desc){
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
    }
}

ecs_entity_t EntRenderPointcloud;

void Pointcloud_OnSet(ecs_iter_t *it)
{
    Pointcloud *state = ecs_field(it, Pointcloud, 1);
    for(int i = 0; i < it->count; ++i, ++state)
    {

    }
}



void RenderingsImport(ecs_world_t *world)
{
    ECS_MODULE(world, Renderings);
    ECS_COMPONENT_DEFINE(world, RenderPointcloud);
    ECS_OBSERVER(world, RenderPointcloud_OnSet, EcsOnSet, RenderPointcloud);
    ECS_OBSERVER(world, Pointcloud_OnSet, EcsOnSet, Pointcloud);

    EntRenderPointcloud = ecs_new(world, 0);
    ecs_set(world, EntRenderPointcloud, RenderPointcloud, {0});
}