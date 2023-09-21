#pragma once
#include <flecs.h>
#include <easymath/mathtypes.h>

typedef struct
{
	qf32 q; // Orientation quaternion
	m4f32 vp; // View Projection matrix
	m4f32 p; // Projection matrix
	v3f32 moving; // Moving direction
	v3f32 looking; // Looking direction
	float moving_speed;
	float looking_speed;
} CamerasCamera;

extern ECS_COMPONENT_DECLARE(CamerasCamera);

void CamerasImport(ecs_world_t *world);