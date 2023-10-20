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

#define M4(i, j) ((i) * 4 + (j))
#define M3(i, j) ((i) * 3 + (j))
void mul3(double *r, double const *a, double const *x);

void m4_print(double const *a);

void m3_print(double const *a);

void v3_print(double const *a);


#ifdef __cplusplus
}
#endif

#endif // OUSTER_MATH_H

/** @} */