#include "viz/misc.h"
#include <platform/log.h>

void print_entity(ecs_world_t * world, ecs_entity_t e)
{
	ecs_entity_t scope = ecs_get_scope(world);
	char const * scope_name = scope ? ecs_get_name(world, scope) : "";
	char *path_str = ecs_get_fullpath(world, e);
	char *type_str = ecs_type_str(world, ecs_get_type(world, e));
	platform_log(ECS_GREY"%s"ECS_NORMAL" %s [%s]\n", scope_name, path_str, type_str);
	ecs_os_free(type_str);
	ecs_os_free(path_str);
}


void print_entity_from_it(ecs_iter_t * it, int i)
{
	ecs_entity_t s = it->system ? ecs_get_parent(it->world, it->system) : 0;
	ecs_entity_t parent = s ? ecs_get_parent(it->world, s) : ecs_get_scope(it->world);
	char const * scope_name = parent ? ecs_get_name(it->world, parent) : "";
	char *path_str = ecs_get_fullpath(it->world, it->entities[i]);
	char *type_str = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
	platform_log(ECS_MAGENTA"%s"ECS_NORMAL" %s [%s]\n", scope_name, path_str, type_str);
	ecs_os_free(type_str);
	ecs_os_free(path_str);
}