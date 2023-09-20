#pragma once
#include <flecs.h>
#include <easymath/mathtypes.h>

typedef struct
{
	qf32 q; // Orientation quaternion

	float mvp[16];	// For shader
	float proj[16]; // Projection matrix

	float move[3]; //
	float look[3];

	float move_speed;
	float look_speed;
} CamerasCamera;

extern ECS_COMPONENT_DECLARE(CamerasCamera);

void CamerasImport(ecs_world_t *world);