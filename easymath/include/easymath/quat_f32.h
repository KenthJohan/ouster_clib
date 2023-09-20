#include "mathtypes.h"
#include "lin_f32.h"
#include <assert.h>
#include <stdio.h>

static void qf32_cpy (qf32 * r, qf32 const * a)
{
	r->x = a->x;
	r->y = a->y;
	r->z = a->z;
	r->w = a->w;
}

static void qf32_identity (qf32 * q)
{
	q->x = 0.0f;
	q->y = 0.0f;
	q->z = 0.0f;
	q->w = 1.0f;
}


static float qf32_norm2 (qf32 * q)
{
	return v4f32_norm2 ((v4f32 *)q);
}


static float qf32_norm (qf32 const * q)
{
	return v4f32_norm ((v4f32 *)q);
}


static void qf32_normalize (qf32 * r, qf32 const * q)
{
	float l = sqrtf(V4_DOT(q->e, q->e));
	r->x = q->x / l;
	r->y = q->y / l;
	r->z = q->z / l;
	r->w = q->w / l;
	// does not work in release mode O3:
	// v4f32_normalize ((v4f32 *)r, (v4f32 const *)q);
}


static void qf32_xyza (qf32 * q, float x, float y, float z, float a)
{
	float const c = cosf (a * 0.5f);
	float const s = sinf (a * 0.5f);
	assert((c != 0) || (s != 0));
	q->x = s * x;
	q->y = s * y;
	q->z = s * z;
	q->w = c;
	//printf("%f %f %f %f, %f %f\n", x, y, z, a, c, s);
	//printf("q:[%f %f %f %f]\n", q->x, q->y, q->z, q->w);
	//vf32_normalize (4, q, q);
	//float n = vf32_norm (4, q);
	//printf ("%f\n", n);
}


static void qf32_axis_angle (qf32 * q, v3f32 const * v, float angle)
{
	qf32_xyza (q, v->x, v->y, v->z, angle);
}


static void qf32_mul1 (qf32 * r, qf32 const * p, qf32 const * q)
{
	//ASSERT_BOOL (r != p);
	//ASSERT_BOOL (r != q);
	r->x = (p->w * q->x) + (p->x * q->w) + (p->y * q->z) - (p->z * q->y);
	r->y = (p->w * q->y) - (p->x * q->z) + (p->y * q->w) + (p->z * q->x);
	r->z = (p->w * q->z) + (p->x * q->y) - (p->y * q->x) + (p->z * q->w);
	r->w = (p->w * q->w) - (p->x * q->x) - (p->y * q->y) - (p->z * q->z);
}


/*
static void qf32_mul2 (qf32 * r, qf32 const * p, qf32 const * q)
{
	r->x = p->w * q->x + p->x * q->w;
	r->y = p->w * q->y - p->x * q->z;
	r->z = p->w * q->z + p->x * q->y;
	r->w = p->w * q->w - p->x * q->x;
}
*/




static void qf32_mul (qf32 * r, qf32 const * p, qf32 const * q)
{
	qf32 t = {0};
	t.x = (p->w * q->x) + (p->x * q->w) + (p->y * q->z) - (p->z * q->y);
	t.y = (p->w * q->y) - (p->x * q->z) + (p->y * q->w) + (p->z * q->x);
	t.z = (p->w * q->z) + (p->x * q->y) - (p->y * q->x) + (p->z * q->w);
	t.w = (p->w * q->w) - (p->x * q->x) - (p->y * q->y) - (p->z * q->z);
	(*r) = t;
}



static void qf32_unit_to_m4 (qf32 const * q, m4f32 * r)
{
	float a = q->w;
	float b = q->x;
	float c = q->y;
	float d = q->z;
	float a2 = a * a;
	float b2 = b * b;
	float c2 = c * c;
	float d2 = d * d;

	r->m11 = a2 + b2 - c2 - d2;
	r->m21 = 2.0f * (b*c + a*d);
	r->m31 = 2.0f * (b*d - a*c);

	r->m12 = 2.0f * (b*c - a*d);
	r->m22 = a2 - b2 + c2 - d2;
	r->m32 = 2.0f * (c*d + a*b);

	r->m13 = 2.0f * (b*d + a*c);
	r->m23 = 2.0f * (c*d - a*b);
	r->m33 = a2 - b2 - c2 + d2;
}


static void qf32_unit_m3 (m3f32 * r, qf32 const * q)
{
	float a = q->w;
	float b = q->x;
	float c = q->y;
	float d = q->z;
	float a2 = a * a;
	float b2 = b * b;
	float c2 = c * c;
	float d2 = d * d;

	r->m11 = a2 + b2 - c2 - d2;
	r->m21 = 2.0f * (b*c + a*d);
	r->m31 = 2.0f * (b*d - a*c);

	r->m12 = 2.0f * (b*c - a*d);
	r->m22 = a2 - b2 + c2 - d2;
	r->m32 = 2.0f * (c*d + a*b);

	r->m13 = 2.0f * (b*d + a*c);
	r->m23 = 2.0f * (c*d - a*b);
	r->m33 = a2 - b2 - c2 + d2;
}


static void qf32_m4 (m4f32 * r, qf32 const * q)
{
	float const l = qf32_norm (q);
	float const s = (l > 0.0f) ? (2.0f / l) : 0.0f;

	float const x = q->x;
	float const y = q->y;
	float const z = q->z;
	float const w = q->w;

	float const xx = s * x * x;
	float const xy = s * x * y;
	float const xz = s * x * z;
	float const xw = s * x * w;

	float const yy = s * y * y;
	float const yz = s * y * z;
	float const yw = s * y * w;

	float const zz = s * z * z;
	float const zw = s * z * w;

	r->m11 = 1.0f - yy - zz;
	r->m22 = 1.0f - xx - zz;
	r->m33 = 1.0f - xx - yy;

	r->m12 = xy - zw;
	r->m21 = xy + zw;

	r->m23 = yz - xw;
	r->m32 = yz + xw;

	r->m31 = xz - yw;
	r->m13 = xz + yw;
}


static void qf32_m3 (m3f32 * r, qf32 const * q)
{
	float const l = qf32_norm (q);
	float const s = (l > 0.0f) ? (2.0f / l) : 0.0f;

	float const x = q->x;
	float const y = q->y;
	float const z = q->z;
	float const w = q->w;

	float const xx = s * x * x;
	float const xy = s * x * y;
	float const xz = s * x * z;
	float const xw = s * x * w;

	float const yy = s * y * y;
	float const yz = s * y * z;
	float const yw = s * y * w;

	float const zz = s * z * z;
	float const zw = s * z * w;

	r->m11 = 1.0f - yy - zz;
	r->m22 = 1.0f - xx - zz;
	r->m33 = 1.0f - xx - yy;

	r->m12 = xy - zw;
	r->m21 = xy + zw;

	r->m23 = yz - xw;
	r->m32 = yz + xw;

	r->m31 = xz - yw;
	r->m13 = xz + yw;
}




/*
https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
void rotate_vector_by_quaternion(const Vector3& v, const Quaternion& q, Vector3& vprime)
{
	// Extract the vector part of the quaternion
	Vector3 u(q.x, q.y, q.z);

	// Extract the scalar part of the quaternion
	float s = q.w;

	// Do the math
	vprime = 2.0f * dot(u, v) * u
		  + (s*s - dot(u, u)) * v
		  + 2.0f * s * cross(u, v);
}

static void qf32_rotate_vector_fixthis (qf32 u, v3f32 y)
{
	v3f32 v;
	v3f32_cpy (&v, y);
	float uv = vf32_dot (3, u, v);
	float ww = u[3] * u[3];
	vf32_set1 (3, y, 0.0f);
	vsf32_macc (3, y, u, 2.0f * uv);
	vsf32_macc (3, y, v, ww - uv);
	v3f32_crossacc_scalar (y, 2.0f * ww, u, v);
}
*/


/*
https://github.com/datenwolf/linmath.h/blob/382ba71905c2c09f10684d19cb5a3fcadf1aba39/linmath.h#L494
Method by Fabian 'ryg' Giessen (of Farbrausch)
t = 2 * cross(q.xyz, v)
v' = v + q.w * t + cross(q.xyz, t)
*/
static void qf32_rotate_vector (qf32 const * q, v3f32 const * v, v3f32 * r)
{
	ASSERT_BOOL (v != r);
	v3f32 t;
	v3f32 u = {{q->x, q->y, q->z}};
	v3f32_cross (&t, (v3f32 *)q, v);
	v3f32_mul (&t, &t, 2.0f);
	v3f32_cross (&u, (v3f32 *)q, &t);
	v3f32_mul (&t, &t, q->w);
	v3f32_add (r, v, &t);
	v3f32_add (r, r, &u);
}


static void qf32_rotate_v3f32 (qf32 * q, v3f32 * v)
{
	v3f32 r;
	qf32_rotate_vector (q, v, &r);
	v3f32_cpy (v, &r);
}


static void qf32_rotate_v3f32_array (qf32 * q, v3f32 v[], uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		qf32_rotate_v3f32 (q, v + i);
	}
}


static void qf32_rotate1_xyza (qf32 * q, float x, float y, float z, float a)
{
	qf32 u;
	qf32_xyza (&u, x, y, z, a);
	qf32_mul (q, &u, q);
}

static void qf32_rotate2_xyza (qf32 * q, float x, float y, float z, float a)
{
	qf32 u;
	qf32_xyza (&u, x, y, z, a);
	qf32_mul (q, q, &u); // q = q * u
}



// yaw (Z), pitch (Y), roll (X)
static void qf32_ypr (qf32 * q, float yaw, float pitch, float roll)
{
	// Abbreviations for the various angular functions
	float cy = cos(yaw * 0.5);
	float sy = sin(yaw * 0.5);
	float cp = cos(pitch * 0.5);
	float sp = sin(pitch * 0.5);
	float cr = cos(roll * 0.5);
	float sr = sin(roll * 0.5);
	q->x = sr * cp * cy - cr * sp * sy;
	q->y = cr * sp * cy + sr * cp * sy;
	q->z = cr * cp * sy - sr * sp * cy;
	q->w = cr * cp * cy + sr * sp * sy;
}


//http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
static void qf32_from_m4 (qf32 * q, m4f32 * m)
{
	q->w = sqrtf (1.0f + m->m11 + m->m22 + m->m33) / 2.0f;
	float w4 = (4.0f * q->w);
	q->x = (m->m32 - m->m21) / w4;
	q->y = (m->m13 - m->m31) / w4;
	q->z = (m->m21 - m->m12) / w4;
}


//http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
static void qf32_from_m3 (qf32 * q, m3f32 * m)
{
	q->w = sqrtf (1.0f + m->m11 + m->m22 + m->m33) / 2.0f;
	float w4 = (4.0f * q->w);
	q->x = (m->m32 - m->m21) / w4;
	q->y = (m->m13 - m->m31) / w4;
	q->z = (m->m21 - m->m12) / w4;
}

