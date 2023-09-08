#pragma once
#include "mathtypes.h"
#include <math.h>

static void m4f32_perspective1 (m4f32 * m, float fov, float aspect, float n, float f)
{
	float a = 1.0f / tan (fov / 2.0f);
	//Column vector 1:
	m->m11 = a / aspect;
	m->m21 = 0.0f;
	m->m31 = 0.0f;
	m->m41 = 0.0f;
	//Column vector 2:
	m->m12 = 0.0f;
	m->m22 = a;
	m->m32 = 0.0f;
	m->m42 = 0.0f;
	//Column vector 3:
	m->m13 = 0.0f;
	m->m23 = 0.0f;
	m->m33 = -((f + n) / (f - n));
	m->m43 = -1.0f;
	//Column vector 4:
	m->m14 = 0.0f;
	m->m24 = 0.0f;
	m->m34 = -((2.0f * f * n) / (f - n));
	m->m44 = 0.0f;
}


static void m4f32_translation3 (m4f32 * m, v3f32 const * t)
{
	//Translation vector in 4th column
	m->m14 = t->x;
	m->m24 = t->y;
	m->m34 = t->z;
}