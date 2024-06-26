#pragma once

#include "sokol_gfx.h"
#include "gmath.h"
#include "types.h"





typedef struct {
	sg_pass_action pass_action;
	sg_buffer vbuf;
	sg_pipeline pip;
	float point_size;
	int vertices_cap;
	int vertices_count;
	vertex_t * vertices;
} draw_points_t;

void draw_points_pass(draw_points_t *app, m4f32 *vp);

void draw_points_init(draw_points_t *app);