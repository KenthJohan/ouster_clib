#pragma once
#include <ouster_clib/sock.h>
#include <ouster_clib/client.h>
#include <ouster_clib/types.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lut.h>

#include <platform/log.h>
#include <platform/net.h>
#include <platform/fs.h>

#include <viz/Geometries.h>
#include <viz/Pointclouds.h>

#include <easymath/lin_f32.h>

#include <flecs.h>

#include <stdlib.h>
#include <stdio.h>


typedef enum
{
	FIELD_RANGE,
	FIELD_IR,
	FIELD_COUNT
} field_t;

typedef enum
{
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;


typedef struct
{
	char const * metafile;
} ouster_app_desc_t;

typedef struct
{
	int socks[SOCK_INDEX_COUNT];
	ecs_os_thread_t thread;				 // Receives UDP packages from LiDAR sensor
	ecs_os_mutex_t lock;				 // Used for thread safe pointcloud copy from socket thread to main thread
	ouster_field_t fields1[FIELD_COUNT]; // Thread producer, uses (lock)
	ouster_field_t fields2[FIELD_COUNT]; // Thread consumer, uses (lock)
	int image_points_size;
	double *image_points_data;
	ouster_lut_t lut;
	ouster_meta_t meta;
} ouster_app_t;


ouster_app_t * ouster_app_init(ouster_app_desc_t * desc);