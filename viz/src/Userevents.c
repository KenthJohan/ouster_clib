#include "viz/Userevents.h"


ECS_COMPONENT_DECLARE(UsereventsInput);

void UsereventsImport(ecs_world_t *world)
{
    ECS_MODULE(world, Userevents);
    ECS_COMPONENT_DEFINE(world, UsereventsInput);
}