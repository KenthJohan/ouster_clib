#include "convert.h"
#include <stdlib.h>

void minmax(uint32_t const *data, int n, uint64_t *out_min, uint64_t *out_max)
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



void convert_u32_to_bmp(uint32_t const *src, Tigr *dst, int w, int h)
{
	uint64_t min;
	uint64_t max;
	minmax(src, w*h, &min, &max);
	uint64_t d = max - min;
	d = (d == 0) ? 1 : d;

	TPixel *td = dst->pix;
	for (int y = 0; y < h; ++y, td += w, src += w) {
		for (int x = 0; x < w; ++x) {
			uint32_t a = src[x];
			a -= min;
			a *= 255;
			a /= d;
			td[x].a = 0xFF;
			td[x].r = a;
			td[x].g = a;
			td[x].b = a;
		}
	}
}