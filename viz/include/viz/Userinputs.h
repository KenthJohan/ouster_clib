#pragma once
#include <flecs.h>



#define USEREVENTS_KEYS_MAX 512
typedef struct
{
    uint8_t keys[512];
} UserinputsKeys;


extern ECS_COMPONENT_DECLARE(UserinputsKeys);


void UserinputsImport(ecs_world_t *world);