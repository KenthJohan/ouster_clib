#pragma once
#include "ouster_clib/types.h"
#include "ouster_clib/mat.h"
#include "ouster_clib/meta.h"


typedef struct 
{
    int frame_id;
    int last_mid;
} ouster_lidar_context_t;


void ouster_lidar_context_get_range(ouster_lidar_context_t * ctx, ouster_meta_t * meta, char const * buf, ouster_mat4_t * mat);