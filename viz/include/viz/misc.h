#pragma once
#include <flecs.h>

#define ENTITY_COLOR "#003366"

void print_entity(ecs_world_t * world, ecs_entity_t e);
void print_entity_from_it(ecs_iter_t * it, int i);