#include "viz/viz.h"
#include "viz/Userinputs.h"
#include "viz/Windows.h"

#include <sokol/sokol_app.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_gl.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_debugtext.h>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static void init(viz_state_t *state)
{
	ecs_world_t *world = state->world;

	sg_setup(&(sg_desc){
		.context = sapp_sgcontext(),
		.logger.func = slog_func,
	});
	sgl_setup(&(sgl_desc_t){.logger.func = slog_func});

	sdtx_setup(&(sdtx_desc_t){
		.fonts[0] = sdtx_font_oric(),
		.logger.func = slog_func,
	});
	//__dbgui_setup(sapp_sample_count());
	ecs_singleton_set(world, RenderingsContext, {0});
}

static void frame(viz_state_t *state)
{
	ecs_world_t *world = state->world;

	const sg_pass_action pass_action = {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.1f, 0.1f, 0.1f, 1.0f}}};
	sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

	ecs_progress(world, 0);
	
	sg_end_pass();
	sg_commit();
}

static void cleanup(viz_state_t *state)
{
	sgl_shutdown();
	sg_shutdown();
}

void event(const sapp_event *evt, viz_state_t *state)
{
	ecs_world_t *world = state->world;
	UserinputsKeys *input = ecs_singleton_get_mut(world, UserinputsKeys);

	switch (evt->type)
	{
	case SAPP_EVENTTYPE_MOUSE_DOWN:
		if (evt->mouse_button == SAPP_MOUSEBUTTON_LEFT)
		{
		}
		// mouse_down(&input->mouse.left);

		if (evt->mouse_button == SAPP_MOUSEBUTTON_RIGHT)
		{
		}
		// mouse_down(&input->mouse.right);
		break;
	case SAPP_EVENTTYPE_MOUSE_UP:
		if (evt->mouse_button == SAPP_MOUSEBUTTON_LEFT)
		{
		}
		// mouse_up(&input->mouse.left);

		if (evt->mouse_button == SAPP_MOUSEBUTTON_RIGHT)
		{
		}
		// mouse_up(&input->mouse.right);
		break;
	case SAPP_EVENTTYPE_MOUSE_SCROLL:
		break;
	case SAPP_EVENTTYPE_KEY_UP:
		assert(evt->key_code < USEREVENTS_KEYS_MAX);
		input->keys[evt->key_code] = 0;
		// key_up(key_get(input, key_code(evt->key_code)));
		break;
	case SAPP_EVENTTYPE_KEY_DOWN:
		assert(evt->key_code < USEREVENTS_KEYS_MAX);
		input->keys[evt->key_code] = 1;
		// key_down(key_get(input, key_code(evt->key_code)));
		break;
	case SAPP_EVENTTYPE_RESIZED:
	{
		break;
	}
	default:
		break;
	}
}

void viz_run(viz_state_t *state)
{
	sapp_run(&(sapp_desc){
		.user_data = state,
		.init_userdata_cb = (void (*)(void *))init,
		.frame_userdata_cb = (void (*)(void *))frame,
		.cleanup_userdata_cb = (void (*)(void *))cleanup,
		.event_userdata_cb = (void (*)(const sapp_event *, void *))event,
		.width = 800,
		.height = 600,
		.sample_count = 4,
		.window_title = "Default window title",
		.icon.sokol_default = true,
		.logger.func = slog_func,
	});
}

void viz_init(viz_state_t *state)
{
}
