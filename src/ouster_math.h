/**
 * @defgroup math Math
 * @brief Functionality for math
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_MATH_H
#define OUSTER_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#define OUSTER_M4(i, j) ((i) * 4 + (j))
#define OUSTER_M3(i, j) ((i) * 3 + (j))

#define OUSTER_REAL_FORMAT "%+20.10f"

#define OUSTER_V3_ARGS(x) (x)[0], (x)[1], (x)[2]
#define OUSTER_V4_ARGS(x) (x)[0], (x)[1], (x)[2], (x)[3]

#define OUSTER_V3_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT "\n"
#define OUSTER_V4_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT OUSTER_REAL_FORMAT "\n"

#define OUSTER_V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define OUSTER_V4_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])

#define OUSTER_M3_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8]
#define OUSTER_M3_ARGS_2(x) (x)[0], (x)[3], (x)[6], (x)[1], (x)[4], (x)[7], (x)[2], (x)[5], (x)[8]
#define OUSTER_M4_ARGS_1(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], (x)[15]
#define OUSTER_M4_ARGS_2(x) (x)[0], (x)[4], (x)[8], (x)[12], (x)[1], (x)[5], (x)[9], (x)[13], (x)[2], (x)[6], (x)[10], (x)[14], (x)[3], (x)[7], (x)[11], (x)[15]

#define OUSTER_M3_FORMAT OUSTER_V3_FORMAT OUSTER_V3_FORMAT OUSTER_V3_FORMAT
#define OUSTER_M4_FORMAT OUSTER_V4_FORMAT OUSTER_V4_FORMAT OUSTER_V4_FORMAT OUSTER_V4_FORMAT

void ouster_m3f64_mul(double r[9], double const a[9], double const x[9]);

void ouster_m4_print(double const a[16]);

void ouster_m3_print(double const a[9]);

void ouster_v3_print(double const a[3]);


#ifdef __cplusplus
}
#endif

#endif // OUSTER_MATH_H

/** @} */