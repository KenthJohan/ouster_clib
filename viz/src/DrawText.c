#include "viz/DrawText.h"
#include "viz/Geometries.h"
#include "viz/Cameras.h"
#include "vendor/sokol_gfx.h"
#include "vendor/sokol_debugtext.h"
#include "vendor/sokol_app.h"


ECS_COMPONENT_DECLARE(Text);

static void RenderText(ecs_iter_t *it)
{
	Text *text = ecs_field(it, Text, 1);
	Position2 *pos = ecs_field(it, Position2, 2);
	for (int i = 0; i < it->count; ++i, ++text, ++pos)
	{
        sdtx_pos(pos->x, pos->y);
        sdtx_puts(text->content);
	}
}

static void Update0(ecs_iter_t *it)
{
    sdtx_canvas(sapp_width(), sapp_height());
    const sg_pass_action pass_action = (sg_pass_action) {
        .colors[0].load_action = SG_LOADACTION_DONTCARE ,
        .depth.load_action = SG_LOADACTION_DONTCARE,
        .stencil.load_action = SG_LOADACTION_DONTCARE
    };
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
}


static void Update1(ecs_iter_t *it)
{
    sdtx_draw();
    sg_end_pass();

}


void DrawTextImport(ecs_world_t *world)
{
    ECS_MODULE(world, DrawText);
    ECS_IMPORT(world, Geometries);

    ECS_COMPONENT_DEFINE(world, Text);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = Update0
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = RenderText,
        .query.filter.terms =
        {
            { .id = ecs_id(Text) },
            { .id = ecs_id(Position2) }
        }
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = Update1
    });

	ecs_struct(world, {
	.entity = ecs_id(Text),
	.members = {
	{ .name = "content", .type = ecs_id(ecs_string_t) },
	}
	});

    /*
    ecs_entity_t e = ecs_new(world, 0);
    ecs_set(world, e, Text, {"Hello!"});
    ecs_set(world, e, Position2, {0,0});
    */
}