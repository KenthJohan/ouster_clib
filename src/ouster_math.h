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
void ouster_m3f64_mul(double r[9], double const a[9], double const x[9]);

void ouster_m4_print(double const a[16]);

void ouster_m3_print(double const a[9]);

void ouster_v3_print(double const a[3]);


#ifdef __cplusplus
}
#endif

#endif // OUSTER_MATH_H

/** @} */