#include "ouster_clib/ouster_field.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_os_api.h"

#include <string.h>

void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t *meta)
{
	ouster_assert_notnull(fields);
	ouster_assert_notnull(meta);
	ouster_field_t *f = fields;
	for (int i = 0; i < count; ++i, f++) {
		ouster_assert(f->depth > 0, "");
		f->rows = meta->pixels_per_column;
		f->cols = meta->midw;
		f->rowsize = f->cols * f->depth;
		f->size = f->rows * f->cols * f->depth;
		f->data = ouster_os_calloc(f->size);
	}
}

void ouster_field_fini(ouster_field_t fields[], int count)
{
	ouster_assert_notnull(fields);
	ouster_field_t *f = fields;
	for (int i = 0; i < count; ++i, f++) {
		ouster_os_free(f->data);
		memset(f->data, 0, sizeof(ouster_field_t));
	}
}

void ouster_field_cpy(ouster_field_t dst[], ouster_field_t src[], int count)
{
	ouster_assert_notnull(dst);
	ouster_assert_notnull(src);
	// memcpy(dst, src, sizeof(ouster_field_t) * count);
	for (int i = 0; i < count; ++i, ++dst, ++src) {
		memcpy(dst->data, src->data, src->size);
	}
}

/*
https://static.ouster.dev/sdk-docs/reference/lidar-scan.html#staggering-and-destaggering
*/
void ouster_destagger(void *data, int cols, int rows, int depth, int rowsize, int pixel_shift_by_row[])
{
	ouster_assert_notnull(data);
	ouster_assert_notnull(pixel_shift_by_row);
	char *row = data;
	for (int irow = 0; irow < rows; ++irow, row += rowsize) {
		int offset = (pixel_shift_by_row[irow] + cols) % cols;
		memmove(row + depth * offset, row, depth * (cols - offset));
		memmove(row, row + depth * (cols - offset), depth * offset);
	}
}

void ouster_field_destagger(ouster_field_t fields[], int count, ouster_meta_t *meta)
{
	ouster_assert_notnull(fields);
	ouster_assert_notnull(meta);
	for (int i = 0; i < count; ++i, ++fields) {
		ouster_destagger(fields->data, fields->cols, fields->rows, fields->depth, fields->rowsize, meta->pixel_shift_by_row);
	}
}

void ouster_field_apply_mask_u32(ouster_field_t *field, ouster_meta_t *meta)
{
	ouster_assert_notnull(field);
	ouster_assert_notnull(meta);
	ouster_extract_t const *extract = meta->extract + field->quantity;
	uint32_t mask = extract->mask;
	if (mask == 0xFFFFFFFF) {
		return;
	}
	int rows = field->rows;
	int cols = field->cols;
	int cells = rows * cols;

	ouster_assert(field->depth == 4, "Destination data depth other than 4 is not supported");
	switch (field->depth) {
	case 1: {
		ouster_assert(mask <= UINT8_MAX, "Mask is too big");
		uint8_t *data8 = (uint8_t *)field->data;
		for (int i = 0; i < cells; ++i) {
			uint16_t value = data8[i];
			value &= (uint16_t)mask;
			data8[i] = value;
		}
	} break;
	case 2: {
		ouster_assert(mask <= UINT16_MAX, "Mask is too big");
		uint16_t *data16 = (uint16_t *)field->data;
		for (int i = 0; i < cells; ++i) {
			uint16_t value = data16[i];
			value &= (uint16_t)mask;
			data16[i] = value;
		}
	} break;
	case 4: {
		ouster_assert(mask <= UINT32_MAX, "Mask is too big");
		uint32_t *data32 = (uint32_t *)field->data;
		for (int i = 0; i < cells; ++i) {
			uint32_t value = data32[i];
			value &= mask;
			data32[i] = value;
		}
	} break;
	}
}

void ouster_field_zero(ouster_field_t fields[], int count)
{
	ouster_assert_notnull(fields);
	for (int i = 0; i < count; ++i, ++fields) {
		memset(fields->data, 0, fields->size);
	}
}
