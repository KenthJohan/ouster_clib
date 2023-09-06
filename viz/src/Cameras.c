#include "viz/Cameras.h"
#include "viz/Userevents.h"

#include "vendor/HandmadeMath.h"
#include "vendor/sokol_app.h"



ECS_COMPONENT_DECLARE(Camera);


float ry;

void Camera_OnUpdate(ecs_iter_t *it)
{
    Camera *cam = ecs_field(it, Camera, 1);
    UsereventsInput *input = ecs_field(it, UsereventsInput, 2);
    const float frame_time = (float)(sapp_frame_duration());
    for(int i = 0; i < it->count; ++i, ++cam)
    {
        if(input->keys[SAPP_KEYCODE_W])
        {
            cam->pos[2]++;
        }
        if(input->keys[SAPP_KEYCODE_S])
        {
            cam->pos[2]--;
        }
        if(input->keys[SAPP_KEYCODE_D])
        {
            cam->pos[0]++;
        }
        if(input->keys[SAPP_KEYCODE_A])
        {
            cam->pos[0]--;
        }
        // model-view-projection matrix
        hmm_mat4 proj = HMM_Perspective(60.0f, sapp_widthf()/sapp_heightf(), 0.01f, 50.0f);
        hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 12.0f), HMM_Vec3(cam->pos[0], cam->pos[1], cam->pos[2]), HMM_Vec3(0.0f, 1.0f, 0.0f));
        hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
        ry += 60.0f * frame_time;
        hmm_mat4 mvp = HMM_MultiplyMat4(view_proj, HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f)));
        ecs_os_memcpy_t(cam->mvp, &mvp, hmm_mat4);
    }
}




void CamerasImport(ecs_world_t *world)
{
    ECS_MODULE(world, Cameras);
    ECS_IMPORT(world, Userevents);
    ECS_COMPONENT_DEFINE(world, Camera);
    ECS_SYSTEM(world, Camera_OnUpdate, EcsOnUpdate, Camera, UsereventsInput($));

	ecs_struct(world, {
	.entity = ecs_id(Camera),
	.members = {
	{ .name = "pos", .type = ecs_id(ecs_f32_t), .count = 3 },
	}
	});

}