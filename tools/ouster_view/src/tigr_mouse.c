#include "tigr/tigr_mouse.h"

void tigr_mouse_get(Tigr *screen, tigr_mouse_t *mouse)
{
	int b;
	tigrMouse(screen, &mouse->x, &mouse->y, &b);
	mouse->up = (mouse->btn ^ b) & ~b;
	mouse->down = (mouse->btn ^ b) & b;
	mouse->btn = b;
}