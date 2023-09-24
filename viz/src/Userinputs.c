#include "viz/Userinputs.h"

ECS_COMPONENT_DECLARE(UserinputsKeys);

ECS_CTOR(UserinputsKeys, ptr, {
	ecs_os_memset_t(ptr, 0, UserinputsKeys);
})

void UserinputsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Userinputs);
	ECS_COMPONENT_DEFINE(world, UserinputsKeys);

	ecs_set_hooks(world, UserinputsKeys, {
		.ctor = ecs_ctor(UserinputsKeys),
		});
}