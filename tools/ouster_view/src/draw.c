#include "draw.h"
#include <stdlib.h>

void minmax(uint32_t *data, int n, uint64_t *out_min, uint64_t *out_max)
{
	uint64_t min = UINT64_MAX;
	uint64_t max = 0;
	for (int i = 0; i < n; ++i) {
		uint64_t a = data[i];
		if (a < min) {
			min = a;
		}
		if (a > max) {
			max = a;
		}
	}
	out_min[0] = min;
	out_max[0] = max;
}

void remap(uint8_t *dst, uint32_t *src, int n)
{
	uint64_t min;
	uint64_t max;
	minmax(src, n, &min, &max);
	for (int i = 0; i < n; ++i) {
		uint64_t a = src[i];
		uint64_t b = a - min;
		b = (b * 255) / ((max - min));
		dst[i] = b;
	}
}

void image_saver_init(image_saver_t *s)
{
	s->comp = 1;
	s->n = s->w * s->h;
	s->image = (uint8_t *)malloc(s->n * s->comp);
}

void image_saver_save(image_saver_t *s, uint32_t *lidar_image)
{
	remap(s->image, lidar_image, s->n);
}