#include "viz/Windows.h"

#include "vendor/sokol_app.h"

ECS_COMPONENT_DECLARE(Window);
ECS_COMPONENT_DECLARE(RenderingsContext);
ECS_COMPONENT_DECLARE(Draw);
ECS_TAG_DECLARE(Update);
ECS_TAG_DECLARE(Invalid);
ECS_TAG_DECLARE(Valid);
ECS_TAG_DECLARE(Setup);

void it_update(ecs_iter_t *it)
{
	Window *window = ecs_field(it, Window, 1);
	for (int i = 0; i < it->count; ++i, ++window)
	{
		window->dt = sapp_frame_duration();
		window->w = sapp_width();
		window->h = sapp_height();
	}
}

void it_set(ecs_iter_t *it)
{
	Window *window = ecs_field(it, Window, 1);
	for (int i = 0; i < it->count; ++i, ++window)
	{
		sapp_set_window_title(window->title);
	}
}

ECS_CTOR(Window, ptr, {
	ecs_os_memset_t(ptr, 0, Window);
	ptr->title = "Default Window Title";
})

void WindowsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Windows);

	ECS_COMPONENT_DEFINE(world, Window);
	ECS_COMPONENT_DEFINE(world, RenderingsContext);
	ECS_COMPONENT_DEFINE(world, Draw);
	ECS_TAG_DEFINE(world, Update);
	ECS_TAG_DEFINE(world, Invalid);
	ECS_TAG_DEFINE(world, Valid);
	ECS_TAG_DEFINE(world, Setup);

	ECS_OBSERVER(world, it_set, EcsOnSet, Window, Valid);

	ecs_system_init(world, &(ecs_system_desc_t){
							   .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
							   .callback = it_update,
							   .query.filter.terms =
								   {
									   {.id = ecs_id(Window)},
									   {.id = Valid}}});

	ecs_struct(world, {.entity = ecs_id(Window),
					   .members = {
						   {.name = "w", .type = ecs_id(ecs_i32_t)},
						   {.name = "h", .type = ecs_id(ecs_i32_t)},
						   {.name = "dt", .type = ecs_id(ecs_f32_t)},
						   {.name = "title", .type = ecs_id(ecs_string_t)},
					   }});

	ecs_set_hooks(world, Window, {
									 .ctor = ecs_ctor(Window),
								 });
}