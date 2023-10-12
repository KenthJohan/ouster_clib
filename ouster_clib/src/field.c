#include "ouster_clib/field.h"
#include "ouster_clib/ouster_assert.h"
#include <stdlib.h>
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
		f->data = calloc(1, fields->size);
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
template <typename T>
inline img_t<T> destagger(const Eigen::Ref<const img_t<T>>& img,
                          const std::vector<int>& pixel_shift_by_row,
                          bool inverse) {
    const size_t h = img.rows();
    const size_t w = img.cols();

    if (pixel_shift_by_row.size() != h)
        throw std::invalid_argument{"image height does not match shifts size"};

    img_t<T> destaggered{h, w};
    for (size_t u = 0; u < h; u++) {
        const std::ptrdiff_t offset =
            ((inverse ? -1 : 1) * pixel_shift_by_row[u] + w) % w;

        destaggered.row(u).segment(offset, w - offset) =
            img.row(u).segment(0, w - offset);
        destaggered.row(u).segment(0, offset) =
            img.row(u).segment(w - offset, offset);
    }
    return destaggered;
}
*/

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
	ouster_extract_t *extract = meta->extract + field->quantity;
	uint32_t mask = extract->mask;
	if (mask == 0xFFFFFFFF) {
		return;
	}

	ouster_assert(field->depth == 4, "Destination data depth other than 4 is not supported");
	if (field->depth == 4) {
		uint32_t *data32 = (uint32_t *)field->data;
		int rows = field->rows;
		int cols = field->cols;
		int cells = rows * cols;
		for (int i = 0; i < cells; ++i) {
			data32[i] &= mask;
		}
	}
}

void ouster_field_zero(ouster_field_t fields[], int count)
{
	ouster_assert_notnull(fields);
	for (int i = 0; i < count; ++i, ++fields) {
		memset(fields->data, 0, fields->size);
	}
}
