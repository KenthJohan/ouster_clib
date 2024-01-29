#include "ouster_math.h"
#include "ouster_clib/ouster_assert.h"
#include <stdio.h>

#define OUSTER_REAL_FORMAT "%+20.10f"

#define OUSTER_V3_ARGS(x) (x)[0], (x)[1], (x)[2]
#define OUSTER_V3_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT "\n"
#define OUSTER_V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

#define OUSTER_V4_ARGS(x) (x)[0], (x)[1], (x)[2], (x)[3]
#define OUSTER_V4_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT "\n"
#define OUSTER_V4_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])

#define OUSTER_M3_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8]
#define OUSTER_M3_ARGS_2(x) (x)[0], (x)[3], (x)[6], (x)[1], (x)[4], (x)[7], (x)[2], (x)[5], (x)[8]
#define OUSTER_M3_FORMAT OUSTER_V3_FORMAT OUSTER_V3_FORMAT OUSTER_V3_FORMAT

#define OUSTER_M4_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], (x)[15]
#define OUSTER_M4_ARGS_2(x) (x)[0], (x)[4], (x)[8], (x)[12], (x)[1], (x)[5], (x)[9], (x)[13], (x)[2], (x)[6], (x)[10], (x)[14], (x)[3], (x)[7], (x)[11], (x)[15]
#define OUSTER_M4_FORMAT OUSTER_V4_FORMAT OUSTER_V4_FORMAT OUSTER_V4_FORMAT OUSTER_V4_FORMAT




void ouster_m4_print(double const a[16])
{
	printf(OUSTER_M4_FORMAT, OUSTER_M4_ARGS_1(a));
}

void ouster_m3_print(double const a[9])
{
	printf(OUSTER_M3_FORMAT, OUSTER_M3_ARGS_1(a));
}

void ouster_v3_print(double const a[3])
{
	printf(OUSTER_V3_FORMAT, OUSTER_V3_ARGS(a));
}

void ouster_m3f64_mul(double r[9], double const a[9], double const x[9])
{
	ouster_assert_notnull(r);
	ouster_assert_notnull(a);
	ouster_assert_notnull(x);
	double temp[3];
	temp[0] = (a[OUSTER_M3(0, 0)] * x[0]) + (a[OUSTER_M3(1, 0)] * x[1]) + (a[OUSTER_M3(2, 0)] * x[2]);
	temp[1] = (a[OUSTER_M3(0, 1)] * x[0]) + (a[OUSTER_M3(1, 1)] * x[1]) + (a[OUSTER_M3(2, 1)] * x[2]);
	temp[2] = (a[OUSTER_M3(0, 2)] * x[0]) + (a[OUSTER_M3(1, 2)] * x[1]) + (a[OUSTER_M3(2, 2)] * x[2]);
	r[0] = temp[0];
	r[1] = temp[1];
	r[2] = temp[2];
}