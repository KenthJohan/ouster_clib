#include "gcamera.h"
#include <assert.h>
#include <stdio.h>

static void quaternion_rotation_procedure(float const look[3], float q[4], float speed)
{
	assert(fabsf(V4_DOT(q, q) - 1.0f) < 0.1f);             // Check quaternion valididy:
	float q_pitch[4];                                      // Quaternion pitch rotation
	float q_yaw[4];                                        // Quaternion yaw rotation
	float q_roll[4];            
	//printf("look %f %f %f\n", V3_ARG(look));                           // Quaternion roll rotation
	qf32_xyza(q_pitch, 1.0f, 0.0f, 0.0f, look[0] * speed); // Make pitch quaternion
	qf32_xyza(q_yaw, 0.0f, 1.0f, 0.0f, look[1] * speed);   // Make yaw quaternion
	qf32_xyza(q_roll, 0.0f, 0.0f, 1.0f, look[2] * speed);  // Make roll quaternion
	qf32_normalize(q, q);
	qf32_mul(q, q_roll, q);  // Apply roll rotation
	qf32_mul(q, q_yaw, q);   // Apply yaw rotation
	qf32_mul(q, q_pitch, q); // Apply pitch rotation
	qf32_normalize(q, q);    // Normalize quaternion against floating point error
	//printf("q %f %f %f %f\n\n", V4_ARG(q));
}

void gcamera_update(gcamera_state_t *camera, float delta, float w, float h)
{
	// This rotates quaternion as air-plane mode:
	quaternion_rotation_procedure(camera->looking, camera->q, delta * camera->looking_speed);

	// Convert unit quaternion to rotation matrix (r)
	m4f32 r = M4_IDENTITY;
	qf32_unit_to_m4(camera->q, &r);

	// Translate postion (pos) relative to direction of camera rotation
	float dir[3];
	dir[0] = V3_DOT(r.c0, camera->moving);
	dir[1] = V3_DOT(r.c1, camera->moving);
	dir[2] = V3_DOT(r.c2, camera->moving);
	v3f32_mul(dir, dir, delta * camera->moving_speed);
	v3f32_add(camera->pos, camera->pos, dir);

	// Make projection matrix (p)
	float rad2deg = (2.0f * M_PI) / 360.0f;
	float aspect = w / h;
	m4f32 p;// Projection matrix
	m4f32_perspective1(&p, camera->fov * rad2deg, aspect, 0.01f, 10000.0f);

	// Apply translation (t), rotation (r), projection - which creates the view-projection-matrix (vp).
	// The view-projection-matrix can then be later used in shaders.
	m4f32 t = M4_IDENTITY;
	m4f32_translation3(&t, camera->pos);

	m4f32_mul(&r, &r, &t);
	//m4f32_print(&r);

	m4f32_mul(&camera->vp, &p, &r);
}



void gcamera_init(gcamera_state_t *camera)
{
	camera->q[0] = 0;
	camera->q[1] = 0;
	camera->q[2] = 0;
	camera->q[3] = 1;
	//camera->looking_speed = 0.8f;
	//camera->moving_speed = 0.1f;
	camera->looking_speed = 0.8f;
	camera->moving_speed = 100.1f;
	camera->fov = 45.0f;
}


