#include "ouster_clib/field.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*

static const Table<ChanField, FieldInfo, 8> legacy_field_info{{
	{ChanField::RANGE, {UINT32, 0, 0x000fffff, 0}},
	{ChanField::FLAGS, {UINT8, 3, 0, 4}},
	{ChanField::REFLECTIVITY, {UINT16, 4, 0, 0}},
	{ChanField::SIGNAL, {UINT16, 6, 0, 0}},
	{ChanField::NEAR_IR, {UINT16, 8, 0, 0}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
	{ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
	{ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 5> lb_field_info{{
	{ChanField::RANGE, {UINT16, 0, 0x7fff, -3}},
	{ChanField::FLAGS, {UINT8, 1, 0b10000000, 7}},
	{ChanField::REFLECTIVITY, {UINT8, 2, 0, 0}},
	{ChanField::NEAR_IR, {UINT8, 3, 0, -4}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 13> dual_field_info{{
	{ChanField::RANGE, {UINT32, 0, 0x0007ffff, 0}},
	{ChanField::FLAGS, {UINT8, 2, 0b11111000, 3}},
	{ChanField::REFLECTIVITY, {UINT8, 3, 0, 0}},
	{ChanField::RANGE2, {UINT32, 4, 0x0007ffff, 0}},
	{ChanField::FLAGS2, {UINT8, 6, 0b11111000, 3}},
	{ChanField::REFLECTIVITY2, {UINT8, 7, 0, 0}},
	{ChanField::SIGNAL, {UINT16, 8, 0, 0}},
	{ChanField::SIGNAL2, {UINT16, 10, 0, 0}},
	{ChanField::NEAR_IR, {UINT16, 12, 0, 0}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
	{ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
	{ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
	{ChanField::RAW32_WORD4, {UINT32, 12, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 8> single_field_info{{
	{ChanField::RANGE, {UINT32, 0, 0x0007ffff, 0}},
	{ChanField::FLAGS, {UINT8, 2, 0b11111000, 3}},
	{ChanField::REFLECTIVITY, {UINT8, 4, 0, 0}},
	{ChanField::SIGNAL, {UINT16, 6, 0, 0}},
	{ChanField::NEAR_IR, {UINT16, 8, 0, 0}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
	{ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
	{ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
}};

static const Table<ChanField, FieldInfo, 14> five_word_pixel_info{{
	{ChanField::RANGE, {UINT32, 0, 0x0007ffff, 0}},
	{ChanField::FLAGS, {UINT8, 2, 0b11111000, 3}},
	{ChanField::REFLECTIVITY, {UINT8, 3, 0, 0}},
	{ChanField::RANGE2, {UINT32, 4, 0x0007ffff, 0}},
	{ChanField::FLAGS2, {UINT8, 6, 0b11111000, 3}},
	{ChanField::REFLECTIVITY2, {UINT8, 7, 0, 0}},
	{ChanField::SIGNAL, {UINT16, 8, 0, 0}},
	{ChanField::SIGNAL2, {UINT16, 10, 0, 0}},
	{ChanField::NEAR_IR, {UINT16, 12, 0, 0}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
	{ChanField::RAW32_WORD2, {UINT32, 4, 0, 0}},
	{ChanField::RAW32_WORD3, {UINT32, 8, 0, 0}},
	{ChanField::RAW32_WORD4, {UINT32, 12, 0, 0}},
	{ChanField::RAW32_WORD5, {UINT32, 16, 0, 0}},
}};

*/

#define COMBINE(p, q) (((p) << 0) | ((q) << 8))

void ouster_field_init1(ouster_field_t *f, int profile)
{
	switch (COMBINE(profile, f->quantity))
	{
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_RANGE):
		f->mask = UINT32_C(0x0007ffff);
		f->offset = 0;
		f->depth = 4;
		break;
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_REFLECTIVITY):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 4;
		f->depth = 1;
		break;
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_SIGNAL):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 6;
		f->depth = 2;
		break;
	case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_NEAR_IR):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 8;
		f->depth = 2;
		break;
		/*
	{ChanField::RANGE, {UINT16, 0, 0x7fff, -3}},
	{ChanField::FLAGS, {UINT8, 1, 0b10000000, 7}},
	{ChanField::REFLECTIVITY, {UINT8, 2, 0, 0}},
	{ChanField::NEAR_IR, {UINT8, 3, 0, -4}},
	{ChanField::RAW32_WORD1, {UINT32, 0, 0, 0}},
	*/
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_RANGE):
		f->mask = UINT32_C(0x7fff);
		f->offset = 0;
		f->depth = 2;
		break;
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_REFLECTIVITY):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 2;
		f->depth = 1;
		break;
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_SIGNAL):
		assert(0);
		break;
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_NEAR_IR):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 3;
		f->depth = 1;
		break;
	default:
		break;
	}
}

void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t *meta)
{
	ouster_field_t *f = fields;
	for (int i = 0; i < count; ++i, f++)
	{
		int rows = meta->pixels_per_column;
		int cols = meta->midw;
		ouster_field_init1(f, meta->profile);
		f->rowsize = cols * f->depth;
		f->data_size = f->rowsize * rows;
		f->data = calloc(f->data_size, 1);
	}
}

void ouster_field_cpy(ouster_field_t dst[], ouster_field_t src[], int count)
{
	// memcpy(dst, src, sizeof(ouster_field_t) * count);
	for (int i = 0; i < count; ++i, ++dst, ++src)
	{
		memcpy(dst->data, src->data, src->data_size);
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
void destagger(void *data, int cols, int rows, int depth, int rowsize, int pixel_shift_by_row[])
{
	char *row = data;
	for (int irow = 0; irow < rows; ++irow, row += rowsize)
	{
		int offset = (pixel_shift_by_row[irow] + cols) % cols;
		memmove(row + depth * offset, row, depth * (cols - offset));
		memmove(row, row + depth * (cols - offset), depth * offset);
	}
}

void ouster_field_destagger(ouster_field_t fields[], int count, ouster_meta_t *meta)
{
	for (int i = 0; i < count; ++i, ++fields)
	{
		int rows = meta->pixels_per_column;
		int cols = meta->midw;
		destagger(fields->data, cols, rows, fields->depth, fields->rowsize, meta->pixel_shift_by_row);
	}
}

void ouster_field_apply_mask_u32(ouster_field_t *field, ouster_meta_t *meta)
{
	if (field->mask == 0xFFFFFFFF)
	{
		return;
	}
	if (field->depth != 4)
	{
		return;
	}
	uint32_t *data32 = (uint32_t *)field->data;
	int rows = meta->pixels_per_column;
	int cols = meta->midw;
	for (int i = 0; i < rows * cols; ++i)
	{
		data32[i] &= field->mask;
	}
}

void ouster_field_zero(ouster_field_t fields[], int count)
{
	for (int i = 0; i < count; ++i, ++fields)
	{
		memset(fields->data, 0, fields->data_size);
	}
}
