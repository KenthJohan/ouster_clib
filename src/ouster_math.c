#include "ouster_math.h"
#include "ouster_clib/ouster_assert.h"


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