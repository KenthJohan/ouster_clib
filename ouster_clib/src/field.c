#include "ouster_clib/field.h"
#include "ouster_clib/mat.h"
#include <string.h>


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

#define COMBINE(p,q) (((p) << 0) | ((q) << 8))

void ouster_field_init1(ouster_field_t * f, int profile)
{
    switch (COMBINE(profile, f->quantity))
    {
    case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_RANGE):
        f->mask = UINT32_C(0x0007ffff);
        f->offset = 0;
        f->mat.dim[0] = 4;
        break;
    case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_REFLECTIVITY):
        f->mask = 0;
        f->offset = 4;
        f->mat.dim[0] = 1;
        break;
    case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_SIGNAL):
        f->mask = 0;
        f->offset = 6;
        f->mat.dim[0] = 2;
        break;
    case COMBINE(OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16, OUSTER_QUANTITY_NEAR_IR):
        f->mask = 0;
        f->offset = 8;
        f->mat.dim[0] = 2;
        break;
    default:
        break;
    }
}





void ouster_field_init(ouster_field_t fields[], int count, ouster_meta_t * meta)
{
    ouster_field_t * f = fields;
    int cols = meta->column_window[1] - meta->column_window[0] + 1;
    meta->xxx_column_offset = meta->column_window[0];

    for(int i = 0; i < count; ++i, f++)
    {
        f->num_valid_pixels = 0;
        ouster_field_init1(f, meta->profile);
        f->mat.dim[1] = cols;
        f->mat.dim[2] = meta->pixels_per_column;
        f->mat.dim[3] = 1;
        ouster_mat4_init(&f->mat);
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
void ouster_field_destagger(ouster_mat4_t * mat, ouster_meta_t * meta)
{
    for(int i = 0; i < meta->pixels_per_column; ++i)
    {
        int e = mat->step[0];
        int w = mat->dim[1];
        int offset = (meta->pixel_shift_by_row[i] + w) % w;
        char * row = mat->data + i * mat->step[1];
        memmove(row + e*offset, row, e*(w - offset));
        memmove(row, row + e*(w - offset), e*offset);
    }
}
