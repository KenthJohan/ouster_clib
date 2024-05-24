#pragma once
#include "gcamera.h"
#include "draw_points.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ouster_clib.h>

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef enum {
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

#define KEY_PRESS 0x1
#define KEY_PRESSED 0x2

typedef struct {
	uint8_t keys[512];
	gcamera_state_t camera;
	double * points_xyz;
	int points_count;
	draw_points_t draw_points;
	pthread_t thread;
	pthread_mutex_t lock;
	float dt;
	float w;
	float h;
	float gui_point_radius; // The graphical point radius in 3D-view
	char const *metafile;
	ouster_meta_t meta;
	int pause;
} app_t;