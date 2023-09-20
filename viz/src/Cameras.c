#include "viz/Cameras.h"
#include "viz/Userinputs.h"
#include "viz/Geometries.h"

#include <sokol/HandmadeMath.h>
#include <sokol/sokol_app.h>

#include <easymath/mathtypes.h>
#include <easymath/lin_f32.h>
#include <easymath/quat_f32.h>
#include <easymath/transform.h>
#include <stdio.h>

ECS_COMPONENT_DECLARE(CamerasCamera);

static void Camera_Controller(ecs_iter_t *it)
{
	// EG_ITER_INFO(it);
	CamerasCamera *camera = ecs_field(it, CamerasCamera, 1);
	UserinputsKeys *input = ecs_field(it, UserinputsKeys, 2);
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

static void rotation_procedure(v3f32 const *look, qf32 *q, float speed)
{
	assert(fabsf(qf32_norm2(q) - 1.0f) < 0.1f);				// Check quaternion valididy:
	qf32 q_pitch;											// Quaternion pitch rotation
	qf32 q_yaw;												// Quaternion yaw rotation
	qf32 q_roll;											// Quaternion roll rotation
	qf32_xyza(&q_pitch, 1.0f, 0.0f, 0.0f, look->x * speed); // Make pitch quaternion
	qf32_xyza(&q_yaw, 0.0f, 1.0f, 0.0f, look->y * speed);	// Make yaw quaternion
	qf32_xyza(&q_roll, 0.0f, 0.0f, 1.0f, look->z * speed);	// Make roll quaternion
	qf32_normalize (q, q);
	qf32_mul(q, &q_roll, q);								// Apply roll rotation
	qf32_mul(q, &q_yaw, q);									// Apply yaw rotation
	qf32_mul(q, &q_pitch, q);								// Apply pitch rotation
	qf32_normalize(q, q);									// Normalize quaternion against floating point error
}


static void Camera_Update(ecs_iter_t *it)
{
	CamerasCamera *camera = ecs_field(it, CamerasCamera, 1);
	Position3 *pos = ecs_field(it, Position3, 2);
	for (int i = 0; i < it->count; ++i, ++camera, ++pos)
	{
		v3f32 *move = (v3f32 *)(camera->move);
		v3f32 *look = (v3f32 *)(camera->look);
		m4f32 *proj = (m4f32 *)(camera->proj);
		m4f32 *mvp = (m4f32 *)(camera->mvp);

		rotation_procedure(look, &camera->q, it->delta_time * camera->look_speed);

		m4f32 mr = {M4_IDENTITY};
		qf32_unit_to_m4(&camera->q, &mr); // Convert unit quaternion to rotation matrix (mr)

		v3f32 dir;
		v3f32_m4_mul(&dir, &mr, move); // Multiply rotation matrix (mr) with move vector (move) to velocity vector (v)
		v3f32_mul(&dir, &dir, it->delta_time * camera->move_speed);
		v3f32_add((v3f32 *)pos, (v3f32 *)pos, &dir);

		float rad2deg = (2.0f * M_PI) / 360.0f;
		float aspect = sapp_widthf() / sapp_heightf();
		m4f32_perspective1(proj, 45.0f * rad2deg, aspect, 0.01f, 10000.0f);

		m4f32 t = {M4_IDENTITY};
		m4f32_translation3(&t, (v3f32 *)pos);
		m4f32_mul(&mr, &mr, &t);
		m4f32_mul(mvp, proj, &mr);
	}
}


ECS_CTOR(CamerasCamera, ptr, {
	ecs_os_memset_t(ptr, 0, CamerasCamera);
	qf32_identity(&ptr->q);
})


void CamerasImport(ecs_world_t *world)
{
	ECS_MODULE(world, Cameras);
	ecs_set_name_prefix(world, "Cameras");
	ECS_IMPORT(world, Userinputs);
	ECS_IMPORT(world, Geometries);
	ECS_COMPONENT_DEFINE(world, CamerasCamera);

	ECS_SYSTEM(world, Camera_Controller, EcsOnUpdate, CamerasCamera(self), UserinputsKeys($));
	ECS_SYSTEM(world, Camera_Update, EcsOnUpdate, CamerasCamera(self), Position3(self));

	ecs_set_hooks(world, CamerasCamera, {
		.ctor = ecs_ctor(CamerasCamera),
		});

	ecs_struct(world, {.entity = ecs_id(CamerasCamera),
		.members = {
			{.name = "q", .type = ecs_id(ecs_f32_t), .count = 4},
			{.name = "mvp", .type = ecs_id(ecs_f32_t), .count = 16},
			{.name = "proj", .type = ecs_id(ecs_f32_t), .count = 16},
			{.name = "move", .type = ecs_id(ecs_f32_t), .count = 3},
			{.name = "look", .type = ecs_id(ecs_f32_t), .count = 3},
			{.name = "move_speed", .type = ecs_id(ecs_f32_t)},
			{.name = "look_speed", .type = ecs_id(ecs_f32_t)},
		}});
}