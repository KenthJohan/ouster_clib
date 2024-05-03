
#pragma once

#include <math.h>
#include <assert.h>
#include <stdio.h>

#define MAX(a, b) (((a) > (b)) ? a : b)
#define MIN(a, b) (((a) < (b)) ? a : b)


#define V1_DOT(a, b) ((a)[0] * (b)[0])
#define V2_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1])
#define V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define V4_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])

#define V1_DOTE_(a, b0) ((a)[0] * b0)
#define V2_DOTE_(a, b0, b1) ((a)[0] * b0 + (a)[1] * b1)
#define V3_DOTE_(a, b0, b1, b2) ((a)[0] * b0 + (a)[1] * b1 + (a)[2] * b2)
#define V4_DOTE_(a, b0, b1, b2, b3) ((a)[0] * b0 + (a)[1] * b1 + (a)[2] * b2 + (a)[3] * b3)

#define V1_DOTE(...) V1_DOTE_(__VA_ARGS__)
#define V2_DOTE(...) V2_DOTE_(__VA_ARGS__)
#define V3_DOTE(...) V3_DOTE_(__VA_ARGS__)
#define V4_DOTE(...) V4_DOTE_(__VA_ARGS__)

#define V1_ARG(a) (a[0])
#define V2_ARG(a) (a[0]), (a[1])
#define V3_ARG(a) (a[0]), (a[1]), (a[2])
#define V4_ARG(a) (a[0]), (a[1]), (a[2]), (a[3])

#define M4_R0(a) ((a).c0[0]), ((a).c1[0]), ((a).c2[0]), ((a).c3[0])
#define M4_R1(a) ((a).c0[1]), ((a).c1[1]), ((a).c2[1]), ((a).c3[1])
#define M4_R2(a) ((a).c0[2]), ((a).c1[2]), ((a).c2[2]), ((a).c3[2])
#define M4_R3(a) ((a).c0[3]), ((a).c1[3]), ((a).c2[3]), ((a).c3[3])

#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

typedef struct
{
	float c0[4];
	float c1[4];
	float c2[4];
	float c3[4];
} m4f32;

#define QF32_IDENTITY \
	{                 \
		1, 0, 0, 0    \
	}

#define M4_IDENTITY                                              \
	{                                                            \
		{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, { 0, 0, 0, 1 } \
	}

void qf32_xyza(float q[4], float x, float y, float z, float a);

void qf32_normalize(float r[4], float const q[4]);

void qf32_mul(float r[4], float const p[4], float const q[4]);

void qf32_unit_to_m4(float const q[4], m4f32 *r);

void v3f32_mul(float r[3], float const a[3], float b);

void v3f32_add(float r[3], float const a[3], float const b[3]);

void m4f32_perspective1(m4f32 *m, float fov, float aspect, float n, float f);

void m4f32_translation3(m4f32 *m, float const t[3]);

void m4f32_transpose(m4f32 *x);

void m4f32_mul_transpose(m4f32 *y, m4f32 const *at, m4f32 *const b);

void m4f32_mul(m4f32 *y, m4f32 const *at, m4f32 *const b);

void m4f32_print(m4f32 * x);