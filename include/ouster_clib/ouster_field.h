/**
 * @defgroup field Fields and destagger
 * @brief Provides network functionality
 *
 * \ingroup c
 * @{
 */


#ifndef OUSTER_FIELD_H
#define OUSTER_FIELD_H

#include "ouster_clib/ouster_types.h"
#include "ouster_clib/ouster_meta.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t *meta);
void ouster_field_fini(ouster_field_t fields[], int count);
void ouster_destagger(void *data, int cols, int rows, int depth, int rowsize, int pixel_shift_by_row[]);
void ouster_field_destagger(ouster_field_t fields[], int count, ouster_meta_t *meta);
void ouster_field_apply_mask_u32(ouster_field_t *field, ouster_meta_t *meta);
void ouster_field_zero(ouster_field_t fields[], int count);
void ouster_field_cpy(ouster_field_t dst[], ouster_field_t src[], int count);

#ifdef __cplusplus
}
#endif

#endif // OUSTER_FIELD_H

/** @} */