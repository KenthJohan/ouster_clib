#include "viz/viz.h"
#include "viz/Renderings.h"

#include "vendor/sokol_app.h"
#include "vendor/sokol_gfx.h"
#include "vendor/sokol_log.h"
#include "vendor/sokol_gl.h"
#include "vendor/sokol_glue.h"

#include <stdlib.h>
#include <math.h>





static void init(void* user_data)
{
    viz_state_t * state = user_data;
    ecs_world_t * world = state->world;


    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });
    sgl_setup(&(sgl_desc_t){ .logger.func = slog_func });
    ecs_singleton_set(world, RenderingsContext, {0});

}


static void frame(void* user_data)
{
    viz_state_t * state = user_data;
    ecs_world_t * world = state->world;
    const sg_pass_action pass_action = {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 0.0f, 1.0f, 0.0f, 1.0f }
        }
    };
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sg_end_pass();
    ecs_progress(world, 0);
    sg_commit();
}

static void cleanup(void* user_data)
{
    sgl_shutdown();
    sg_shutdown();
}

void event(const sapp_event* e)
{

}



void viz_run(viz_state_t * state)
{
    sapp_run(&(sapp_desc){
        .user_data = state,
        .init_userdata_cb = init,
        .frame_userdata_cb = frame,
        .cleanup_userdata_cb = cleanup,  // cleanup doesn't need access to the state struct
        .event_cb = event,
        .width = 800,
        .height = 600,
        .sample_count = 4,
        .window_title = "Noentry (sokol-app)",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    });
}




void viz_init(viz_state_t * state)
{

}

