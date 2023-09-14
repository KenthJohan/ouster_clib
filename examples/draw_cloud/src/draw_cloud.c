#include <viz/viz.h>
#include <viz/Geometries.h>
#include <viz/DrawInstances.h>
#include <viz/DrawText.h>
#include <viz/DrawPoints.h>
#include <viz/Cameras.h>
#include <viz/Windows.h>
#include <viz/Pointclouds.h>

#include "Sensors.h"

#include <stdio.h>
#include <assert.h>

#include <platform/fs.h>
#include <platform/log.h>

int main(int argc, char *argv[])
{
	fs_pwd();

	ecs_world_t *world = ecs_init();

	// Must import before loading flecs file
	ECS_IMPORT(world, DrawInstances);
	ECS_IMPORT(world, DrawPoints);
	ECS_IMPORT(world, DrawText);
	ECS_IMPORT(world, Geometries);
	ECS_IMPORT(world, Pointclouds);
	ECS_IMPORT(world, Cameras);
	ECS_IMPORT(world, Windows);
	ECS_IMPORT(world, Sensors);

	// https://www.flecs.dev/explorer/?remote=true
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	ecs_plecs_from_file(world, "profile.flecs");
	// ecs_set_threads(world, 4);

	while (0)
	{
		ecs_progress(world, 0);
	}

	viz_state_t state = {.world = world};
	viz_init(&state);
	viz_run(&state);

	return ecs_fini(world);
}