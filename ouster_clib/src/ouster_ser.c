#include "ouster_clib/ouster_ser.h"
#include "ouster_clib/ouster_log.h"


void ouster_ser_i32(char data[4], int32_t value)
{
	data[0] = ((uint32_t)value >> 24) & 0xFF;
	data[1] = ((uint32_t)value >> 16) & 0xFF;
	data[2] = ((uint32_t)value >> 8) & 0xFF;
	data[3] = ((uint32_t)value >> 0) & 0xFF;
}