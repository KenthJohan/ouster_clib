#include "viz/DrawText.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_debugtext.h>
#include <sokol/sokol_app.h>

ECS_COMPONENT_DECLARE(Text);

static void RenderText(ecs_iter_t *it)
{
	static float temporary_fps = 0;
	Text *text = ecs_field(it, Text, 1);
	Position2 *pos = ecs_field(it, Position2, 2);
	for (int i = 0; i < it->count; ++i, ++text, ++pos)
	{
		// sdtx_origin(1.0f, 2.0f);
		sdtx_pos(pos->x, pos->y);
		sdtx_puts(text->content);
		// Temorary:
		temporary_fps = 0.9 * temporary_fps + 0.1 * (1.0f / it->delta_time);
		sdtx_printf("FPS %i\n", (int)temporary_fps);
	}
}

static void Update0(ecs_iter_t *it)
{
	sdtx_canvas(sapp_width() / 10, sapp_height() / 10);
}

static void Update1(ecs_iter_t *it)
{
	sdtx_draw();
}

void DrawTextImport(ecs_world_t *world)
{
	ECS_MODULE(world, DrawText);
	ECS_IMPORT(world, Geometries);

	ECS_COMPONENT_DEFINE(world, Text);

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Update0});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = RenderText,
		.query.filter.terms =
		{
		{.id = ecs_id(Text)},
		{.id = ecs_id(Position2)}}});

	ecs_system_init(world, &(ecs_system_desc_t){
							   .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
							   .callback = Update1});

	ecs_struct(world, {.entity = ecs_id(Text),
		.members = {
			{.name = "content", .type = ecs_id(ecs_string_t)},
		}});

	/*
	ecs_entity_t e = ecs_new(world, 0);
	ecs_set(world, e, Text, {"Hello!"});
	ecs_set(world, e, Position2, {0,0});
	*/
}