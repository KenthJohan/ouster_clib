#include "viz/Cameras.h"
#include "viz/Userinputs.h"

#include "vendor/HandmadeMath.h"
#include "vendor/sokol_app.h"
#include "mathtypes.h"
#include "lin_f32.h"
#include "quat_f32.h"
#include "transform.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(Camera);


static void Camera_Controller(ecs_iter_t *it)
{
	//EG_ITER_INFO(it);
	Camera         *camera = ecs_field(it, Camera, 1);
	UserinputsKeys *input  = ecs_field(it, UserinputsKeys, 2);
	for (int i = 0; i < it->count; ++i, ++camera)
	{
        camera->look[0] = input->keys[SAPP_KEYCODE_UP] - input->keys[SAPP_KEYCODE_DOWN];
        camera->look[1] = input->keys[SAPP_KEYCODE_RIGHT] - input->keys[SAPP_KEYCODE_LEFT];
        camera->look[2] = input->keys[SAPP_KEYCODE_E] - input->keys[SAPP_KEYCODE_Q];
        camera->move[0] = input->keys[SAPP_KEYCODE_A] - input->keys[SAPP_KEYCODE_D];
        camera->move[1] = input->keys[SAPP_KEYCODE_LEFT_CONTROL] - input->keys[SAPP_KEYCODE_SPACE];
        camera->move[2] = input->keys[SAPP_KEYCODE_W] - input->keys[SAPP_KEYCODE_S];
	}
}

void rot(v3f32 const * look, qf32  * q, float speed)
{
    // Rotation stage
    qf32 q_pitch; // Quaternion pitch rotation
    qf32 q_yaw;   // Quaternion yaw rotation
    qf32 q_roll;  // Quaternion roll rotation
    qf32_xyza (&q_pitch, 1.0f, 0.0f, 0.0f, look->x*speed);
    qf32_xyza (&q_yaw,   0.0f, 1.0f, 0.0f, look->y*speed);
    qf32_xyza (&q_roll,  0.0f, 0.0f, 1.0f, look->z*speed);
    qf32_mul (q, &q_roll, q);  // Apply roll rotation
    qf32_mul (q, &q_yaw, q);   // Apply yaw rotation
    qf32_mul (q, &q_pitch, q); // Apply pitch rotation
    qf32_normalize (q, q); // Normalize quaternion against floating point error
}



static void Camera_Update(ecs_iter_t *it)
{
	//EG_ITER_INFO(it);
	Camera *camera = ecs_field(it, Camera, 1);
	for (int i = 0; i < it->count; ++i, ++camera)
	{
		qf32  * q    = (qf32*)  (camera->q);
		v3f32 * move = (v3f32*) (camera->move);
		v3f32 * look = (v3f32*) (camera->look);
		m4f32 * proj = (m4f32*) (camera->proj);
		m4f32 * mvp = (m4f32*) (camera->mvp);
		v3f32 * pos = (v3f32*) (camera->pos);


        rot(look, q, it->delta_time * 0.5f); // (look,q) -> q



		m4f32 mr = M4F32_IDENTITY;
		qf32_unit_m4 (&mr, q); // Convert unit quaternion to rotation matrix (mr)

        v3f32 dir;
		v3f32_m4_mul (&dir, &mr, move); // Multiply rotation matrix (mr) with move vector (move) to velocity vector (v)
        v3f32_mul (&dir, &dir, it->delta_time * 1.5f);
        v3f32_add (pos, pos, &dir);
        //printf("%f %f %f => %f %f %f => %f %f %f\n", move->x, move->y, move->z, dir.x, dir.y, dir.z, pos->x, pos->y, pos->z);

        float rad2deg = (2.0f*M_PI)/360.0f;
        float aspect = sapp_widthf()/sapp_heightf();
        m4f32_perspective1(proj, 45.0f*rad2deg, aspect, 0.01f, 10000.0f);

        m4f32 t = M4F32_IDENTITY;
        m4f32_translation3(&t, pos);
        m4f32_mul(&mr, &mr, &t);
        m4f32_mul(mvp, proj, &mr);

        //printf("\n");

        //ecs_os_memcpy_t(mvp, proj, m4f32);
	}
}



/*
float ry;

void Camera_OnUpdate(ecs_iter_t *it)
{
    Camera *cam = ecs_field(it, Camera, 1);
    UserinputsKeys *input = ecs_field(it, UserinputsKeys, 2);
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
*/

ECS_CTOR(Camera, ptr, {
    ecs_os_memset_t(ptr, 0, Camera);
    qf32_identity((qf32*)ptr->q);
})


void CamerasImport(ecs_world_t *world)
{
    ECS_MODULE(world, Cameras);
    ECS_IMPORT(world, Userinputs);
    ECS_COMPONENT_DEFINE(world, Camera);

    ECS_SYSTEM(world, Camera_Controller, EcsOnUpdate, Camera, UserinputsKeys($));
    ECS_SYSTEM(world, Camera_Update, EcsOnUpdate, Camera);

    ecs_set_hooks(world, Camera, {
        .ctor = ecs_ctor(Camera),
    });

    /*
	ecs_struct(world, {
	.entity = ecs_id(Camera),
	.members = {
	{ .name = "mvp", .type = ecs_id(ecs_f32_t), .count = 16 },
	}
	});
    */

}