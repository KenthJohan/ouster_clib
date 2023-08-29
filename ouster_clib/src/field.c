#include "ouster_clib/field.h"



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

int ouster_field_range_size(int profile)
{
    switch (profile)
    {
    case OUSTER_PROFILE_LIDAR_LEGACY: return 4;
    case OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL: return 4;
    case OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16: return 4;
    case OUSTER_PROFILE_RNG15_RFL8_NIR8: return 2;
    case OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL: return 4;
    }
    return 0;
}

uint32_t ouster_field_range_offset(int profile)
{
    switch (profile)
    {
    case OUSTER_PROFILE_LIDAR_LEGACY: return 0;
    case OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL: return 0;
    case OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16: return 0;
    case OUSTER_PROFILE_RNG15_RFL8_NIR8: return 0;
    case OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL: return 0;
    }
    return 0;
}

uint32_t ouster_field_range_mask(int profile)
{
    switch (profile)
    {
    case OUSTER_PROFILE_LIDAR_LEGACY: return UINT32_C(0x000fffff);
    case OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL: return UINT32_C(0x0007ffff);
    case OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16: return UINT32_C(0x0007ffff);
    case OUSTER_PROFILE_RNG15_RFL8_NIR8: return UINT32_C(0x7fff);
    case OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL: return UINT32_C(0x0007ffff);
    }
    return 0;
}










void ouster_field_init(ouster_field_t * field, ouster_meta_t * meta)
{
    field->num_valid_pixels = 0;
    switch (field->quantity)
    {
    case OUSTER_QUANTITY_RANGE:
        field->mask = ouster_field_range_mask(meta->profile);
        field->offset = ouster_field_range_offset(meta->profile);
        field->mat.dim[0] = ouster_field_range_size(meta->profile);
        break;
    case OUSTER_QUANTITY_NEAR_IR:
        field->mask = ouster_field_range_mask(meta->profile);
        field->offset = ouster_field_range_offset(meta->profile);
        field->mat.dim[0] = ouster_field_range_size(meta->profile);
        break;

    default:
        break;
    }
    field->mat.dim[1] = meta->pixels_per_column;
    field->mat.dim[2] = meta->column_window[1] - meta->column_window[0] + 1;
    field->mat.dim[3] = 1;
    ouster_mat4_init(&field->mat);
}