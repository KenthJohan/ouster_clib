#pragma once
#include <stdint.h>

void minmax(uint32_t *data, int n, uint64_t *out_min, uint64_t *out_max);

void remap(uint32_t *dst, uint32_t *src, int n);