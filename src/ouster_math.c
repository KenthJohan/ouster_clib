#include "ouster_math.h"
#include "ouster_clib/ouster_assert.h"
#include <stdio.h>

#define FMTF "%+20.10f"
#define FMTF3 FMTF FMTF FMTF "\n"
#define FMTF4 FMTF FMTF FMTF FMTF "\n"

#define M4_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], (x)[15]
#define M4_ARGS_2(x) (x)[0], (x)[4], (x)[8], (x)[12], (x)[1], (x)[5], (x)[9], (x)[13], (x)[2], (x)[6], (x)[10], (x)[14], (x)[3], (x)[7], (x)[11], (x)[15]
#define M4_FORMAT FMTF4 FMTF4 FMTF4 FMTF4

#define M3_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8]
#define M3_ARGS_2(x) (x)[0], (x)[3], (x)[6], (x)[1], (x)[4], (x)[7], (x)[2], (x)[5], (x)[8]
#define M3_FORMAT FMTF3 FMTF3 FMTF3

#define V3_ARGS(x) (x)[0], (x)[1], (x)[2]
#define V3_FORMAT "%f %f %f\n"
#define V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

void m4_print(double const *a)
{
	printf(M4_FORMAT, M4_ARGS_1(a));
}

void m3_print(double const *a)
{
	printf(M3_FORMAT, M3_ARGS_1(a));
}

void v3_print(double const *a)
{
	printf(V3_FORMAT, V3_ARGS(a));
}

void mul3(double *r, double const *a, double const *x)
{
	ouster_assert_notnull(r);
	ouster_assert_notnull(a);
	ouster_assert_notnull(x);
	double temp[3];
	temp[0] = (a[M3(0, 0)] * x[0]) + (a[M3(1, 0)] * x[1]) + (a[M3(2, 0)] * x[2]);
	temp[1] = (a[M3(0, 1)] * x[0]) + (a[M3(1, 1)] * x[1]) + (a[M3(2, 1)] * x[2]);
	temp[2] = (a[M3(0, 2)] * x[0]) + (a[M3(1, 2)] * x[1]) + (a[M3(2, 2)] * x[2]);
	r[0] = temp[0];
	r[1] = temp[1];
	r[2] = temp[2];
}