#pragma once

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

#define V2F32_ZERO {0.0f, 0.0f}
#define V2F32_REPEAT(x) {(x), (x)}
typedef union v2f32
{
	float e[2];
	struct
	{
		float x;
		float y;
	};
} v2f32;


#define V4F32_ZERO {0.0f, 0.0f, 0.0f, 0.0f}
#define V4F32_REPEAT(x) {(x), (x), (x), (x)}
#define V4F32_V3(v) (v4f32){{(v).x, (v).y, (v).z, 0.0f}}
typedef union v4f32
{
	float e[4];
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
} v4f32;




#define V3F32_ZERO {{0.0f, 0.0f, 0.0f}}
#define V3F32_REPEAT(x) {(x), (x), (x)}
#define V3F32_V4F32(x) {(x).x, (x).y, (x).z}
typedef union v3f32
{
	float e[3];
	struct
	{
		float x;
		float y;
		float z;
	};
} v3f32;


#define M3_11 0
#define M3_21 1
#define M3_31 2
#define M3_12 3
#define M3_22 4
#define M3_32 5
#define M3_13 6
#define M3_23 7
#define M3_33 8
#define M3_FORMAT "%3.3f %3.3f %3.3f\n%3.3f %3.3f %3.3f\n%3.3f %3.3f %3.3f\n"
#define M3_ARGS(m) m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8]
#define M3_ARGST(m) m[0],m[3],m[6],m[1],m[4],m[7],m[2],m[5],m[8]
//                         |----Column1-----|-----Column2-----|-----Column3-----|
#define M3F32_ZERO         {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}
#define M3F32_REPEAT(x)    {{ (x),  (x),  (x),  (x),  (x),  (x),  (x),  (x),  (x)}}
#define M3F32_IDENTITY     {{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}}
#define M3F32_DIAGONAL(x)  {{ (x), 0.0f, 0.0f, 0.0f,  (x), 0.0f, 0.0f, 0.0f,  (x)}}
#define M3F32_SCALE(x,y,z) {{ (x), 0.0f, 0.0f, 0.0f,  (y), 0.0f, 0.0f, 0.0f,  (z)}}
typedef union m3f32
{
	float m[9];
	struct
	{
		float m11;
		float m21;
		float m31;
		float m12;
		float m22;
		float m32;
		float m13;
		float m23;
		float m33;
	};
	v3f32 c[3];
} m3f32;








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
//                                 |-------Column1--------|--------Column2--------|--------Column2--------|--------Column3--------|
#define M4F32_ZERO                 {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}
#define M4F32_IDENTITY             {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}
#define M4F32_DIAGONAL(x)          {{ (x), 0.0f, 0.0f, 0.0f, 0.0f,  (x), 0.0f, 0.0f, 0.0f, 0.0f,  (x), 0.0f, 0.0f, 0.0f, 0.0f,  (x)}}
#define M4F32_SCALE(x,y,z,w)       {{ (x), 0.0f, 0.0f, 0.0f, 0.0f,  (y), 0.0f, 0.0f, 0.0f, 0.0f,  (z), 0.0f, 0.0f, 0.0f, 0.0f,  (w)}}
#define M4F32_TRANSLATION(x,y,z)   {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  (x),  (y),  (z), 1.0f}}
typedef union m4f32
{
	float m[16];
	struct
	{
		float m11;
		float m21;
		float m31;
		float m41;
		float m12;
		float m22;
		float m32;
		float m42;
		float m13;
		float m23;
		float m33;
		float m43;
		float m14;
		float m24;
		float m34;
		float m44;
	};
} m4f32;


#define QF32_IDENTITY  {0.0f, 0.0f, 0.0f, 1.0f}
typedef union qf32
{
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
} qf32;