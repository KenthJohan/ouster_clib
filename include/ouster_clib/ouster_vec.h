/**
 * @defgroup vec Growable vector
 * @brief Functionality for appending data to vector
 *
 * \ingroup c
 * @{
 */

#ifndef OUSTER_VEC_H
#define OUSTER_VEC_H
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OUSTER_OFFSET(o, offset) (void *)(((uintptr_t)(o)) + ((uintptr_t)(offset)))

typedef struct
{
	int cap;
	int count;
	int esize;
	void *data;
} ouster_vec_t;

void ouster_vec_init(ouster_vec_t *v, int esize, int cap);

void ouster_vec_append(ouster_vec_t *v, void const *data, int n, float factor);

int test_ouster_vec();

#ifdef __cplusplus
}
#endif

#endif // OUSTER_VEC_H

/** @} */