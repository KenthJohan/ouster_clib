#pragma once
#include <stdint.h>


typedef struct 
{
	int n;
	int w;
	int h;
	int depth;
	int comp;
	uint8_t * image;
	char const *filename;
} image_saver_t;



void image_saver_init(image_saver_t * s);
void image_saver_save(image_saver_t * s, uint32_t * lidar_image);