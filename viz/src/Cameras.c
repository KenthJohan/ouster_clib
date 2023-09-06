#include "viz/Cameras.h"

#include "vendor/HandmadeMath.h"
#include "vendor/sokol_app.h"



ECS_COMPONENT_DECLARE(Camera);


float ry;

void Camera_OnUpdate(ecs_iter_t *it)
{
    Camera *cam = ecs_field(it, Camera, 1);
    const float frame_time = (float)(sapp_frame_duration());
    for(int i = 0; i < it->count; ++i, ++cam)
    {
        // model-view-projection matrix
        hmm_mat4 proj = HMM_Perspective(60.0f, sapp_widthf()/sapp_heightf(), 0.01f, 50.0f);
        hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 12.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
        hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
        ry += 60.0f * frame_time;
        hmm_mat4 mvp = HMM_MultiplyMat4(view_proj, HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f)));
        ecs_os_memcpy_t(cam->mvp, &mvp, hmm_mat4);
    }
}






void CamerasImport(ecs_world_t *world)
{
    ECS_MODULE(world, Cameras);
    ECS_COMPONENT_DEFINE(world, Camera);
    ECS_SYSTEM(world, Camera_OnUpdate, EcsOnUpdate, Camera);
}