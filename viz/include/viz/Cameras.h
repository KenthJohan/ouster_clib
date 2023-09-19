#pragma once
#include <flecs.h>

typedef struct
{
	float speed;

	float mvp[16];	// For shader
	float proj[16]; // Projection matrix

	float q[4]; // Orientation quaternion

	float move[3]; //
	float look[3];
} CamerasCamera;

extern ECS_COMPONENT_DECLARE(CamerasCamera);

void CamerasImport(ecs_world_t *world);