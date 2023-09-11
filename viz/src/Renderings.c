#include "viz/Renderings.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include "viz/Windows.h"
#include "viz/Pointclouds.h"

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
    float ry;
} RenderPointcloud;



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
            .radius = 0.10f,
            .slices = 5,
            .stacks = 3,
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
            .size = rend->cap * sizeof(float) * 3,
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

        ecs_remove_id(it->world, it->entities[i], Setup);
        ecs_add_id(it->world, it->entities[i], Valid);
    }
}






void RenderPointcloud_Draw(ecs_iter_t *it)
{
    Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
    RenderPointcloud *rend = ecs_field(it, RenderPointcloud, 2);
    Camera *cam = ecs_field(it, Camera, 3);
    Window *window = ecs_field(it, Window, 4); // up
    
    for(int i = 0; i < it->count; ++i, ++cloud)
    {
        //printf("%s\n", ecs_get_name(it->world, it->entities[i]));
        if(rend->cap <= 0){continue;}
        if(cloud->count <= 0){continue;}

        // update instance data
        sg_update_buffer(rend->bind.vertex_buffers[1], &(sg_range){
            .ptr = cloud->pos,
            .size = (size_t)cloud->count * sizeof(hmm_vec3)
        });

        sg_begin_default_pass(&rend->pass_action, window->w, window->h);
        sg_apply_pipeline(rend->pip);
        sg_apply_bindings(&rend->bind);

        ecs_os_memcpy_t(&rend->vs_params.mvp, cam->mvp, hmm_mat4);
        sg_range a = { &rend->vs_params, sizeof(vs_params_t) };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &a);
        sg_draw(rend->shapes[BOX].draw.base_element, rend->shapes[i].draw.num_elements, cloud->count);
        sg_end_pass();
    }
}








void RenderingsImport(ecs_world_t *world)
{
    ECS_MODULE(world, Renderings);
    ECS_IMPORT(world, Cameras);
    ECS_IMPORT(world, Geometries);
    ECS_IMPORT(world, Windows);
    ECS_IMPORT(world, Pointclouds);
    

    ECS_COMPONENT_DEFINE(world, RenderPointcloud);


    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = RenderPointcloud_Setup,
        .query.filter.terms =
        {
            { .id = ecs_id(RenderPointcloud) },
            { .id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
            { .id = Setup },
            
            //{ .id = ecs_id(RenderPointcloud), .src.trav = EcsIsA, .src.flags = EcsUp },
            //{ .id = ecs_id(Camera), .src.trav = EcsIsA, .src.flags = EcsUp },
        }
    });


    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = RenderPointcloud_Draw,
        .query.filter.terms =
        {
            { .id = ecs_id(Pointcloud) },
            { .id = ecs_id(RenderPointcloud), .src.trav = EcsIsA, .src.flags = EcsUp },
            { .id = ecs_id(Camera), .src.trav = EcsIsA, .src.flags = EcsUp },
            { .id = ecs_id(Window), .src.trav = EcsIsA, .src.flags = EcsUp },
            { .id = ecs_id(RenderingsContext), .src.id = ecs_id(RenderingsContext)},
            { .id = Valid },
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