#pragma once

#include "gmath.h"

typedef struct
{
	float q[4];        // Orientation quaternion
	m4f32 vp;      // View Projection matrix
	float moving[3];  // Moving direction
	float looking[3]; // Looking direction
	float pos[3]; // Looking direction
	float moving_speed;
	float looking_speed;
	float fov;
} gcamera_state_t;



void gcamera_init(gcamera_state_t *camera);
void gcamera_update(gcamera_state_t *camera, float delta, float w, float h);

