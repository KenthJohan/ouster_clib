#pragma once

typedef union v2f32
{
	float e[2];
	struct
	{
		float x;
		float y;
	};
} v2f32;

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

typedef union qf32
{
	float e[4];
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
} qf32;