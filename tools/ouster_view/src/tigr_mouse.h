#pragma once
#include "tigr.h"

typedef struct
{
	int btn;
	int up;
	int down;
	int x;
	int y;
} tigr_mouse_t;

void tigr_mouse_get(Tigr* screen, tigr_mouse_t * mouse);