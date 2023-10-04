#include "ouster_clib/meta.h"
#include "ouster_basics.h"
#include "ouster_clib/ouster_assert.h"
#include "ouster_clib/ouster_log.h"
#include "ouster_clib/types.h"

#define JSMN_HEADER
#include "jsmn.h"
#include "json.h"
#include <stdlib.h>
#include <string.h>

#define TOK_COUNT 1024

char *jsmn_strerror(int r)
{
	switch (r) {
	case JSMN_ERROR_NOMEM:
		return "JSMN_ERROR_NOMEM";
	case JSMN_ERROR_INVAL:
		return "JSMN_ERROR_INVAL";
	case JSMN_ERROR_PART:
		return "JSMN_ERROR_PART";
	default:
		return "JSMN_ERROR_?";
	}
}

#define STRING_BUF_SIZE 128

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

void ouster_extract_init(ouster_extract_t *f, ouster_profile_t profile, ouster_quantity_t quantity)
{
	ouster_assert_notnull(f);

	switch (COMBINE(profile, quantity)) {
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
	case COMBINE(OUSTER_PROFILE_RNG15_RFL8_NIR8, OUSTER_QUANTITY_NEAR_IR):
		f->mask = UINT32_C(0xFFFFFFFF);
		f->offset = 3;
		f->depth = 1;
		break;
	default:
		break;
	}
}

void ouster_meta_parse(char const *json, ouster_meta_t *out)
{
	ouster_assert_notnull(json);
	ouster_assert_notnull(out);

	jsmn_parser p;
	jsmn_init(&p);
	jsmntok_t tokens[TOK_COUNT];
	int r = jsmn_parse(&p, json, strlen(json), tokens, TOK_COUNT);
	if (r < 1) {
		ouster_log("jsmn error: %s\n", jsmn_strerror(r));
		return;
	}
	ouster_assert(tokens[0].type == JSMN_OBJECT, "Expected JSMN_OBJECT");

	int column_window[2];
	json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "columns_per_frame", NULL}, &out->columns_per_frame, JSON_TYPE_INT);
	json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "columns_per_packet", NULL}, &out->columns_per_packet, JSON_TYPE_INT);
	json_parse_value(json, tokens, (char const *[]){"lidar_data_format", "pixels_per_column", NULL}, &out->pixels_per_column, JSON_TYPE_INT);
	json_parse_vector(json, tokens, (char const *[]){"lidar_data_format", "column_window", NULL}, column_window, 2, JSON_TYPE_INT);
	ouster_assert(out->pixels_per_column > 0, "Not in range pixels_per_column");
	ouster_assert(out->pixels_per_column <= 128, "Not in range pixels_per_column");
	json_parse_vector(json, tokens, (char const *[]){"lidar_data_format", "pixel_shift_by_row", NULL}, out->pixel_shift_by_row, out->pixels_per_column, JSON_TYPE_INT);

	json_parse_value(json, tokens, (char const *[]){"beam_intrinsics", "lidar_origin_to_beam_origin_mm", NULL}, &out->lidar_origin_to_beam_origin_mm, JSON_TYPE_F64);
	json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_altitude_angles", NULL}, out->beam_altitude_angles, out->pixels_per_column, JSON_TYPE_F64);
	json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_azimuth_angles", NULL}, out->beam_azimuth_angles, out->pixels_per_column, JSON_TYPE_F64);
	json_parse_vector(json, tokens, (char const *[]){"beam_intrinsics", "beam_to_lidar_transform", NULL}, out->beam_to_lidar_transform, 16, JSON_TYPE_F64);

	json_parse_vector(json, tokens, (char const *[]){"lidar_intrinsics", "lidar_to_sensor_transform", NULL}, out->lidar_to_sensor_transform, 16, JSON_TYPE_F64);

	char buf[STRING_BUF_SIZE];
	json_parse_string(json, tokens, (char const *[]){"lidar_data_format", "udp_profile_lidar", NULL}, buf, STRING_BUF_SIZE);

	if (strcmp(buf, "LIDAR_LEGACY") == 0) {
		out->profile = OUSTER_PROFILE_LIDAR_LEGACY;
		out->channel_data_size = 12;
	} else if (strcmp(buf, "RNG19_RFL8_SIG16_NIR16_DUAL") == 0) {
		out->profile = OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL;
		out->channel_data_size = 16;
	} else if (strcmp(buf, "RNG19_RFL8_SIG16_NIR16") == 0) {
		out->profile = OUSTER_PROFILE_RNG19_RFL8_SIG16_NIR16;
		out->channel_data_size = 12;
	} else if (strcmp(buf, "RNG15_RFL8_NIR8") == 0) {
		out->profile = OUSTER_PROFILE_RNG15_RFL8_NIR8;
		out->channel_data_size = 4;
	} else if (strcmp(buf, "FIVE_WORD_PIXEL") == 0) {
		out->profile = OUSTER_PROFILE_FIVE_WORDS_PER_PIXEL;
		out->channel_data_size = 20;
	} else {
		ouster_assert(0, "Profile not found");
	}

	/*
	  https://github.com/ouster-lidar/ouster_example/blob/9d0971107f6f9c95e16afd727fa2534d01a0fe4e/ouster_client/src/parsing.cpp#L155

	  col_size = col_header_size + pixels_per_column * channel_data_size +
	              col_footer_size;

	        lidar_packet_size = packet_header_size + columns_per_packet * col_size +
	                            packet_footer_size;

	*/
	out->col_size = OUSTER_COLUMN_HEADER_SIZE + out->pixels_per_column * out->channel_data_size + OUSTER_COLUMN_FOOTER_SIZE;
	out->lidar_packet_size = OUSTER_PACKET_HEADER_SIZE + out->columns_per_packet * out->col_size + OUSTER_PACKET_FOOTER_SIZE;

	out->mid0 = MIN(column_window[0], column_window[1]);
	out->mid1 = MAX(column_window[0], column_window[1]);
	out->midw = abs(column_window[0] - column_window[1]) + 1;

	for (ouster_quantity_t quantity = 0; quantity < OUSTER_QUANTITY_CHAN_FIELD_MAX; ++quantity) {
		ouster_extract_t *e = out->extract + quantity;
		ouster_extract_init(e, out->profile, quantity);
		int rows = out->pixels_per_column;
		int cols = out->midw;
		ouster_assert(rows >= 0, "");
		ouster_assert(cols >= 0, "");
	}

	ouster_assert(out->mid0 < out->mid1, "");
	ouster_assert(out->midw > 0, "");
	ouster_assert(out->channel_data_size > 0, "");
}