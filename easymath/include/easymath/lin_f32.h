#pragma once
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

#include "mathtypes.h"

#ifndef ASSERT_BOOL
#define ASSERT_BOOL(x)
#endif

#ifndef ASSERT_NEQP
#define ASSERT_NEQP(a, b)
#endif

#define V1_DOT(a, b) ((a)[0] * (b)[0])
#define V2_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1])
#define V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define V4_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])

#define V1_NORM2(a) V1_DOT(a,a)
#define V2_NORM2(a) V2_DOT(a,a)
#define V3_NORM2(a) V3_DOT(a,a)
#define V4_NORM2(a) V4_DOT(a,a)

#define V1_REPEAT(x) {(x)}
#define V2_REPEAT(x) {(x), (x)}
#define V3_REPEAT(x) {(x), (x), (x)}
#define V4_REPEAT(x) {(x), (x), (x), (x)}

#define V1_ZERO {0}
#define V2_ZERO {0,0}
#define V3_ZERO {0,0,0}
#define V4_ZERO {0,0,0,0}

#define M3_11 0
#define M3_21 1
#define M3_31 2
#define M3_12 3
#define M3_22 4
#define M3_32 5
#define M3_13 6
#define M3_23 7
#define M3_33 8

#define M4_11 0
#define M4_21 1
#define M4_31 2
#define M4_41 3
#define M4_12 4
#define M4_22 5
#define M4_32 6
#define M4_42 7
#define M4_13 8
#define M4_23 9
#define M4_33 10
#define M4_43 11
#define M4_14 12
#define M4_24 13
#define M4_34 14
#define M4_44 15

#define V1_ARGS(x) (x)[0]
#define V2_ARGS(x) (x)[0], (x)[1]
#define V3_ARGS(x) (x)[0], (x)[1], (x)[2]
#define V4_ARGS(x) (x)[0], (x)[1], (x)[2], (x)[3]

#define V3_FORMAT(f) ""f" "f" "f""
#define V4_FORMAT(f) ""f" "f" "f" "f""

#define M3_FORMAT(f) ""(f)" "(f)" "(f)"\n"(f)" "(f)" "(f)"\n"(f)" "(f)" "(f)"\n"
#define M3_ARGS(m) m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8]
#define M3_ARGST(m) m[0],m[3],m[6],m[1],m[4],m[7],m[2],m[5],m[8]

//                              |--Column1--|--Column2--|--Column3--|
#define M3_ZERO                 { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 }
#define M3_REPEAT(x)            {(x),(x),(x),(x),(x),(x),(x),(x),(x)}
#define M3_IDENTITY             { 1 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 1 }
#define M3_DIAGONAL(x)          {(x), 0 , 0 , 0 ,(x), 0 , 0 , 0 ,(x)}
#define M3_SCALE(x,y,z)         {(x), 0 , 0 , 0 ,(y), 0 , 0 , 0 ,(z)}

//                              |----Column1----|----Column2----|----Column3----|----Column4----|
#define M4_ZERO                 { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 }
#define M4_IDENTITY             { 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 }
#define M4_DIAGONAL(x)          {(x), 0 , 0 , 0 , 0 ,(x), 0 , 0 , 0 , 0 ,(x), 0 , 0 , 0 , 0 ,(x)}
#define M4_SCALE(x,y,z,w)       {(x), 0 , 0 , 0 , 0 ,(y), 0 , 0 , 0 , 0 ,(z), 0 , 0 , 0 , 0 ,(w)}
#define M4_TRANSLATION(x,y,z)   { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,(x),(y),(z), 1 }

#define QUAT_IDENTITY  {0, 0, 0, 1}
#define QUAT_MUL0(a,b) ((a)[3] * (b)[0]) + ((a)[0] * (b)[3]) + ((a)[1] * (b)[2]) - ((a)[2] * (b)[1]);
#define QUAT_MUL1(a,b) ((a)[3] * (b)[1]) - ((a)[0] * (b)[2]) + ((a)[1] * (b)[3]) + ((a)[2] * (b)[0]);
#define QUAT_MUL2(a,b) ((a)[3] * (b)[2]) + ((a)[0] * (b)[1]) - ((a)[1] * (b)[0]) + ((a)[2] * (b)[3]);
#define QUAT_MUL3(a,b) ((a)[3] * (b)[3]) - ((a)[0] * (b)[0]) - ((a)[1] * (b)[1]) - ((a)[2] * (b)[2]);


// r := a < b
static void vvf32_lt (uint32_t n, float r [], float const a [], float const b [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] < b [i];
	}
}


// r := a > b
static void vvf32_gt (uint32_t n, float r [], float const a [], float const b [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] > b [i];
	}
}


// r := a + b
static void vvf32_add (uint32_t n, float r [], float const a [], float const b [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] + b [i];
	}
}


// r := a - b
static void vvf32_sub (uint32_t n, float r [], float const a [], float const b [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] - b [i];
	}
}


static void vf32_addv (uint32_t dim, float y[], uint32_t y_stride, float const a[], uint32_t a_stride, float const b[], uint32_t b_stride, uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		vvf32_add (dim, y, a, b);
		y += y_stride;
		a += a_stride;
		b += b_stride;
	}
}


static void vf32_subv (uint32_t dim, float y[], uint32_t y_stride, float const a[], uint32_t a_stride, float const b[], uint32_t b_stride, uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		vvf32_sub (dim, y, a, b);
		y += y_stride;
		a += a_stride;
		b += b_stride;
	}
}


static void vf32_set1_strided (float v[], float x, uint32_t n, unsigned inc)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		v[0] = x;
		v += inc;
	}
}


static void vf32_set2 (float v[], float v0, float v1)
{
	v[0] = v0;
	v[1] = v1;
}


static void vf32_set3 (float v[], float v0, float v1, float v2)
{
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;
}


static void vf32_set4 (float v[], float v0, float v1, float v2, float v3)
{
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;
	v[3] = v3;
}



// r := a . b
static float vf32_dot (uint32_t n, float const a [], float const b [])
{
	float r = 0.0f;
	for (uint32_t i = 0; i < n; ++i)
	{
		r += a[i] * b[i];
	}
	return r;
}



static void vf32_cpy (uint32_t n, float des[], float const src[])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		des[i] = src[i];
	}
}







// r := r + a
static void vf32_acc (unsigned n, float r [], float const a [])
{
	vvf32_add (n, r, r, a);
}


// r := r - a
static void vf32_decc (unsigned n, float r [], float const a [])
{
	vvf32_sub (n, r, r, a);
}





// ret a > b
static int vf32_gt (uint32_t n, float const a [], float const b [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		if (a[i] < b[i])
		{
			return 0;
		}
	}
	return 1;
}


// ret a < b
static int vf32_lt (unsigned n, float const a[], float const b[])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		if (a[i] < b[i]) {return 0;}
	}
	return 1;
}


// Set all element (x) of r to b
// r := {x | x = ?}
static void vf32_random (uint32_t n, float r [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = (float)rand() / (float)RAND_MAX;
		//r [i] = 1.0f;
	}
}


// Set all element (x) of r to b
// r := {x | x = b}
static void vf32_set1 (uint32_t n, float r [], float const b)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r[i] = b;
	}
}


// r := a * b
static void vvf32_hadamard (unsigned n, float r[], float const a[], float const b[])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r[i] = a[i] * b[i];
	}
}


// r := r + a * b
static void vvf32_macc (uint32_t n, float r [], float const a [], float const b [])
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r[i] += a[i] * b[i];
	}
}


static float vf32_sum (uint32_t n, float const v [])
{
	float sum = 0;
	for (uint32_t i = 0; i < n; ++i)
	{
		sum += v[i];
	}
	return sum;
}










static void vf32_linespace (uint32_t n, float x[], float x1, float x2)
{
	float const d = (x2 - x1) / n;
	float a = x1;
	for (uint32_t i = 0; i < n; ++i)
	{
		x[i] = a;
		a += d;
	}
}


static void vf32_repeat (uint32_t n, float v[], float value, uint32_t stride)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		v[i*stride] = value;
	}
}


static void vf32_weight_ab (uint32_t n, float y[], float a[], float b[], float k)
{
	float A = k;
	float B = 1.0f - k;
	for (uint32_t i = 0; i < n; ++i)
	{
		y[i] = A * a[i] + B * b[i];
	}
}


static void vf32_setl (float r [], uint32_t n, ...)
{
	va_list ap;
	va_start (ap, n);
	for (uint32_t i = 0; i < n; ++i)
	{
		r[i] = va_arg (ap, double);
	}
	va_end (ap);
}


static float vf32_avg (uint32_t n, float v[])
{
	ASSERT_BOOL (n > 0);
	float sum = 0.0f;
	for (uint32_t i = 0; i < n; ++i)
	{
		sum += v[i];
	}
	return sum / n;
}



static float vf32_max (uint32_t n, float v[])
{
	ASSERT_BOOL (n >= 1);
	float max = v[0];
	for (uint32_t i = 0; i < n; ++i)
	{
		if (v[i] > max)
		{
			max = v[i];
		}
	}
	return max;
}


static float vf32_maxabs (uint32_t n, float v[])
{
	ASSERT_BOOL (n >= 1);
	float max = v[0];
	for (uint32_t i = 0; i < n; ++i)
	{
		float x = fabs (v[i]);
		if (x > max)
		{
			max = x;
		}
	}
	return max;
}
























// r := a - b
static void vsf32_sub (unsigned n, float r [], float const a [], float b)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] - b;
	}
}

static void vsf32_macc (uint32_t n, float vy[], float const vx[], float sb)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		vy [i] += vx [i] * sb;
	}
}

// r := a + b
static void vsf32_add (uint32_t n, float r [], float const a [], float const b)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] + b;
	}
}

// r := a + b
static void vsf32_add_max (uint32_t n, float r [], float const a [], float const b, float max)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		if (r [i] < max)
		{
			r [i] = a [i] + b;
		}
	}
}

// r := a * b
static void vsf32_mul (uint32_t n, float r [], float const a [], float const b)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		r [i] = a [i] * b;
	}
}





















// ret |a|^2
static float vf32_norm2 (uint32_t n, float const a [])
{
	return vf32_dot (n, a, a);
}


// ret |a|
static float vf32_norm (uint32_t n, float const a [])
{
	return sqrtf (vf32_norm2 (n, a));
}


// r := a / |a|
static void vf32_normalize (uint32_t n, float r [], float const a [])
{
	float const l = vf32_norm (n, a);
	float const s = l > 0.0f ? 1.0f / l : 0.0f;
	vsf32_mul (n, r, a, s);
}




static void v3f32_set1 (v3f32 * r, float b)
{
	r->x = b;
	r->y = b;
	r->z = b;
}


static void v3f32_set_xyz (v3f32 * r, float x, float y, float z)
{
	r->x = x;
	r->y = y;
	r->z = z;
}


static void v3f32_cpy (v3f32 * r, v3f32 const * a)
{
	r->x = a->x;
	r->y = a->y;
	r->z = a->z;
}


static void v3f32_mul (v3f32 * r, v3f32 const * a, float b)
{
	r->x = a->x * b;
	r->y = a->y * b;
	r->z = a->z * b;
}

static void v3f32_mul_hadamard (v3f32 * r, v3f32 const * a, v3f32 const * b)
{
	r->x = a->x * b->x;
	r->y = a->y * b->y;
	r->z = a->z * b->z;
}


static void v3f32_sub (v3f32 * r, v3f32 const * a, v3f32 const * b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
}

static void v3f32_subv (v3f32 r[], v3f32 const a[], v3f32 const b[], uint32_t incr, uint32_t inca, uint32_t incb, uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		v3f32_sub (r, a, b);
		r += incr;
		a += inca;
		b += incb;
	}
}

static void v3f32_add (v3f32 * r, v3f32 const * a, v3f32 const * b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

static void v3f32_addv (v3f32 r[], v3f32 const a[], v3f32 const b[], uint32_t incr, uint32_t inca, uint32_t incb, uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		v3f32_add (r, a, b);
		r += incr;
		a += inca;
		b += incb;
	}
}


static void v3f32_add_mul (v3f32 * r, v3f32 const * a, v3f32 const * b, float alpha, float beta)
{
	r->x = a->x * alpha + b->x * beta;
	r->y = a->y * alpha + b->y * beta;
	r->z = a->z * alpha + b->z * beta;
}


static void v3f32_cross (v3f32 * r, v3f32 const * a, v3f32 const * b)
{
	r->x = a->y * b->z - a->z * b->y;
	r->y = a->z * b->x - a->x * b->z;
	r->z = a->x * b->y - a->y * b->x;
}


static float v3f32_dot (v3f32 const * a, v3f32 const * b)
{
	float sum = 0;
	sum += a->x * b->x;
	sum += a->y * b->y;
	sum += a->z * b->z;
	return sum;
}


static float v3f32_dotv (float r[], v3f32 const a[], v3f32 const b[], int inca, int incb, int count)
{
	float sum = 0;
	for (int i = 0; i < count; ++i)
	{
		r[i] = (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
		a += inca;
		b += incb;
	}
	return sum;
}



static void v3f32_crossacc (v3f32 * r, v3f32 const * a, v3f32 const * b)
{
	r->x += a->y * b->z - a->z * b->y;
	r->y += a->z * b->x - a->x * b->z;
	r->z += a->x * b->y - a->y * b->x;
}


static void v3f32_crossacc_scalar (v3f32 * r, float s, v3f32 const * a, v3f32 const * b)
{
	r->x += s * (a->y * b->z - a->z * b->y);
	r->y += s * (a->z * b->x - a->x * b->z);
	r->z += s * (a->x * b->y - a->y * b->x);
}


static void v3f32_sum (v3f32 * y, v3f32 x[], uint32_t x_stride, uint32_t x_count)
{
	for (uint32_t i = 0; i < x_count; ++i)
	{
		y->x += x->x;
		y->y += x->y;
		y->z += x->z;
		x += x_stride;
	}
}


static float v3f32_norm2 (v3f32 const * a)
{
	return v3f32_dot (a, a);
}

static float v3f32_norm (v3f32 const * a)
{
	return sqrtf (v3f32_norm2 (a));
}

static void v3f32_normalize (v3f32 * a)
{
	float l = v3f32_norm (a);
	ASSERT_BOOL (l != 0.0f);
	a->x /= l;
	a->y /= l;
	a->z /= l;
}




static int v3f32_ray_sphere_intersect (v3f32 * p, v3f32 * d, v3f32 * sc, float sr, float *t, v3f32 * q)
{
	//Vector m = p - s.c;
	v3f32 m;
	v3f32_sub (&m, p, sc);
	//float b = Dot(m, d);
	float b = v3f32_dot (&m, d);
	//float c = Dot(m, m) - s.r * s.r;
	float c = v3f32_dot (&m, &m) - (sr * sr);

	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
	if (c > 0.0f && b > 0.0f) {return 0;}
	float discr = (b * b) - c;

	// A negative discriminant corresponds to ray missing sphere
	if (discr < 0.0f) {return 0;}

	// Ray now found to intersect sphere, compute smallest t value of intersection
	(*t) = -b - sqrtf (discr);

	// If t is negative, ray started inside sphere so clamp t to zero
	if (*t < 0.0f) {*t = 0.0f;}
	(*t) = ((*t) > 0.0f) ? (*t) : 0.0f;

	//q = p + dt;
	v3f32_mul (q, d, *t);
	v3f32_add (q, q, p);

	return 1;
}


static void v3f32_m4_mul (v3f32 * y, m4f32 const * a, v3f32 * const b)
{
	y->x = (a->m11 * b->x) + (a->m21 * b->y) + (a->m31 * b->z);
	y->y = (a->m12 * b->x) + (a->m22 * b->y) + (a->m32 * b->z);
	y->z = (a->m13 * b->x) + (a->m23 * b->y) + (a->m33 * b->z);
}


static void v3f32_m3_mul (v3f32 * y, m3f32 const * a, v3f32 * const b)
{
	y->x = (a->m11 * b->x) + (a->m21 * b->y) + (a->m31 * b->z);
	y->y = (a->m12 * b->x) + (a->m22 * b->y) + (a->m32 * b->z);
	y->z = (a->m13 * b->x) + (a->m23 * b->y) + (a->m33 * b->z);
}




//Copy all points inside the ball
static uint32_t v3f32_ball (v3f32 const x[], uint32_t n, v3f32 const * c, v3f32 y[], float r)
{
	uint32_t m = 0;
	for(uint32_t i = 0; i < n; ++i)
	{
		v3f32 d;
		v3f32_sub (&d, x + i, c);
		if (v3f32_norm2 (&d) < (r*r))
		{
			y[m] = x[i];
			m++;
		}
	}
	return m;
}







static void v3f32_meanacc (v3f32 * y, v3f32 const x[], uint32_t n)
{
	ASSERT_BOOL (n > 0); //Divide by zero protection
	for (uint32_t i = 0; i < n; ++i)
	{
		y->x += x[i].x;
		y->y += x[i].y;
		y->z += x[i].z;
	}
	float s = 1.0f / n;
	y->x *= s;
	y->y *= s;
	y->z *= s;
}





static void v3f32_lerp2 (v3f32 * y, v3f32 const * a, v3f32 * const b, float t)
{
	y->x = (1 - t) * a->x + t * b->x;
	y->y = (1 - t) * a->y + t * b->y;
	y->z = (1 - t) * a->z + t * b->z;
}


static void v4f32_cpy (v4f32 * r, v4f32 const * a)
{
	r->x = a->x;
	r->y = a->y;
	r->z = a->z;
	r->w = a->w;
}


static void v4f32_add (v4f32 * r, v4f32 const * a, v4f32 const * b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
	r->w = a->w + b->w;
}


static void v4f32_acc (v4f32 * r, v4f32 const * a)
{
	r->x += a->x;
	r->y += a->y;
	r->z += a->z;
	r->w += a->w;
}


static void v4f32_sub (v4f32 * r, v4f32 const * a, v4f32 const * b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
	r->w = a->w - b->w;
}

static void v4f32_subv (v4f32 r[], v4f32 const a[], v4f32 const b[], uint32_t incr, uint32_t inca, uint32_t incb, uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		v4f32_sub (r, a, b);
		r += incr;
		a += inca;
		b += incb;
	}
}


static void v4f32_set1 (v4f32 * r, float b)
{
	r->x = b;
	r->y = b;
	r->z = b;
	r->w = b;
}


static void v4f32_mul (v4f32 * r, v4f32 const * a, float b)
{
	r->x = a->x * b;
	r->y = a->y * b;
	r->z = a->z * b;
	r->w = a->w * b;
}


static void v4f32_div (v4f32 * r, v4f32 const * a, float b)
{
	r->x = a->x / b;
	r->y = a->y / b;
	r->z = a->z / b;
	r->w = a->w / b;
}


static float v4f32_dot (v4f32 const * a, v4f32 const * b)
{
	float sum = 0;
	sum += a->x * b->x;
	sum += a->y * b->y;
	sum += a->z * b->z;
	sum += a->w * b->w;
	return sum;
}


static float v4f32_norm2 (v4f32 const * a)
{
	return v4f32_dot (a, a);
}


static float v4f32_norm (v4f32 const * a)
{
	return sqrtf (v4f32_norm2 (a));
}


static void v4f32_normalize (v4f32 * r, v4f32 const * a)
{
	float l = v4f32_norm (a);
	v4f32_div (r, a, l);
}


static void v4f32_set_xyzw (v4f32 * v, float x, float y, float z, float w)
{
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}




static void v4f32_dotv (v4f32 r[], uint32_t inc_r, v4f32 a[], uint32_t inc_a, v4f32 b[], uint32_t inc_b, uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{

		r += inc_r;
		a += inc_a;
		b += inc_b;
	}
}






static void v4f32_m4_macc_unsafe (v4f32 * y, m4f32 const * a, v4f32 * const b)
{
	ASSERT_NEQP (y, b);
	y->x += (a->m11 * b->x) + (a->m12 * b->y) + (a->m13 * b->z) + (a->m14 * b->w);
	y->y += (a->m21 * b->x) + (a->m22 * b->y) + (a->m23 * b->z) + (a->m24 * b->w);
	y->z += (a->m31 * b->x) + (a->m32 * b->y) + (a->m33 * b->z) + (a->m34 * b->w);
	y->w += (a->m41 * b->x) + (a->m42 * b->y) + (a->m43 * b->z) + (a->m44 * b->w);
}

static void v4f32_m4_macc (v4f32 * y, m4f32 const * a, v4f32 * const b)
{
	v4f32 t;
	v4f32_cpy (&t, y);
	t.x += (a->m11 * b->x) + (a->m12 * b->y) + (a->m13 * b->z) + (a->m14 * b->w);
	t.y += (a->m21 * b->x) + (a->m22 * b->y) + (a->m23 * b->z) + (a->m24 * b->w);
	t.z += (a->m31 * b->x) + (a->m32 * b->y) + (a->m33 * b->z) + (a->m34 * b->w);
	t.w += (a->m41 * b->x) + (a->m42 * b->y) + (a->m43 * b->z) + (a->m44 * b->w);
	v4f32_cpy (y, &t);
}



static void v4f32_m4_macct_unsafe (v4f32 * y, m4f32 const * a, v4f32 * const b)
{
	ASSERT_NEQP (y, b);
	y->x += (a->m11 * b->x) + (a->m21 * b->y) + (a->m31 * b->z) + (a->m41 * b->w);
	y->y += (a->m12 * b->x) + (a->m22 * b->y) + (a->m32 * b->z) + (a->m42 * b->w);
	y->z += (a->m13 * b->x) + (a->m23 * b->y) + (a->m33 * b->z) + (a->m43 * b->w);
	y->w += (a->m14 * b->x) + (a->m24 * b->y) + (a->m34 * b->z) + (a->m44 * b->w);
}

static void v4f32_m4_macct (v4f32 * y, m4f32 const * a, v4f32 * const b)
{
	v4f32 t;
	v4f32_cpy (&t, y);
	t.x += (a->m11 * b->x) + (a->m21 * b->y) + (a->m31 * b->z) + (a->m41 * b->w);
	t.y += (a->m12 * b->x) + (a->m22 * b->y) + (a->m32 * b->z) + (a->m42 * b->w);
	t.z += (a->m13 * b->x) + (a->m23 * b->y) + (a->m33 * b->z) + (a->m43 * b->w);
	t.w += (a->m14 * b->x) + (a->m24 * b->y) + (a->m34 * b->z) + (a->m44 * b->w);
	v4f32_cpy (y, &t);
}

static void v4f32_m4_mul_unsafe (v4f32 * y, m4f32 const * a, v4f32 * const b)
{
	ASSERT_NEQP (y, b);
	y->x = (a->m11 * b->x) + (a->m12 * b->y) + (a->m13 * b->z) + (a->m14 * b->w);
	y->y = (a->m21 * b->x) + (a->m22 * b->y) + (a->m23 * b->z) + (a->m24 * b->w);
	y->z = (a->m31 * b->x) + (a->m32 * b->y) + (a->m33 * b->z) + (a->m34 * b->w);
	y->w = (a->m41 * b->x) + (a->m42 * b->y) + (a->m43 * b->z) + (a->m44 * b->w);
}

static void v4f32_m4_mul (v4f32 * y, m4f32 const * a, v4f32 * const b)
{
	v4f32 t;
	t.x = (a->m11 * b->x) + (a->m12 * b->y) + (a->m13 * b->z) + (a->m14 * b->w);
	t.y = (a->m21 * b->x) + (a->m22 * b->y) + (a->m23 * b->z) + (a->m24 * b->w);
	t.z = (a->m31 * b->x) + (a->m32 * b->y) + (a->m33 * b->z) + (a->m34 * b->w);
	t.w = (a->m41 * b->x) + (a->m42 * b->y) + (a->m43 * b->z) + (a->m44 * b->w);
	v4f32_cpy (y, &t);
}



static void m4f32_mul (m4f32 * y, m4f32 const * a, m4f32 * const b)
{
	m4f32 t;
	t.m11 = (a->m11 * b->m11) + (a->m12 * b->m21) + (a->m13 * b->m31) + (a->m14 * b->m41);
	t.m21 = (a->m21 * b->m11) + (a->m22 * b->m21) + (a->m23 * b->m31) + (a->m24 * b->m41);
	t.m31 = (a->m31 * b->m11) + (a->m32 * b->m21) + (a->m33 * b->m31) + (a->m34 * b->m41);
	t.m41 = (a->m41 * b->m11) + (a->m42 * b->m21) + (a->m43 * b->m31) + (a->m44 * b->m41);

	t.m12 = (a->m11 * b->m12) + (a->m12 * b->m22) + (a->m13 * b->m32) + (a->m14 * b->m42);
	t.m22 = (a->m21 * b->m12) + (a->m22 * b->m22) + (a->m23 * b->m32) + (a->m24 * b->m42);
	t.m32 = (a->m31 * b->m12) + (a->m32 * b->m22) + (a->m33 * b->m32) + (a->m34 * b->m42);
	t.m42 = (a->m41 * b->m12) + (a->m42 * b->m22) + (a->m43 * b->m32) + (a->m44 * b->m42);

	t.m13 = (a->m11 * b->m13) + (a->m12 * b->m23) + (a->m13 * b->m33) + (a->m14 * b->m43);
	t.m23 = (a->m21 * b->m13) + (a->m22 * b->m23) + (a->m23 * b->m33) + (a->m24 * b->m43);
	t.m33 = (a->m31 * b->m13) + (a->m32 * b->m23) + (a->m33 * b->m33) + (a->m34 * b->m43);
	t.m43 = (a->m41 * b->m13) + (a->m42 * b->m23) + (a->m43 * b->m33) + (a->m44 * b->m43);

	t.m14 = (a->m11 * b->m14) + (a->m12 * b->m24) + (a->m13 * b->m34) + (a->m14 * b->m44);
	t.m24 = (a->m21 * b->m14) + (a->m22 * b->m24) + (a->m23 * b->m34) + (a->m24 * b->m44);
	t.m34 = (a->m31 * b->m14) + (a->m32 * b->m24) + (a->m33 * b->m34) + (a->m34 * b->m44);
	t.m44 = (a->m41 * b->m14) + (a->m42 * b->m24) + (a->m43 * b->m34) + (a->m44 * b->m44);

	*y = t;
}

























//Set r = xyzw xyzw xyzw ... n times
static void v4f32_set_xyzw_repeat (uint32_t n, float r [], float x, float y, float z, float w)
{
	while (n--)
	{
		r [0] = x;
		r [1] = y;
		r [2] = z;
		r [3] = w;
		r += 4;
	}
}


static void v4f32_set_w_repeat (uint32_t n, float r [], float w)
{
	while (n--)
	{
		r [3] = w;
		r += 4;
	}
}


static void v4f32_repeat_random (unsigned n, float r [])
{
	uint32_t const dim = 4;
	while (n--)
	{
		vf32_random (dim, r);
		r += dim;
	}
}


static void v4f32_repeat_channel (unsigned n, float r [], unsigned channel, float a)
{
	while (n--)
	{
		r [channel] = a;
		r += 4;
	}
}



