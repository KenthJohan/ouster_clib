#include "viz/viz.h"
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_gl.h"
#include "sokol_glue.h"
#include <stdlib.h>
#include <math.h>





static void init(void* user_data) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });
    sgl_setup(&(sgl_desc_t){ .logger.func = slog_func });
}


static void frame(void* user_data) {
    viz_state_t * state = user_data;
    ecs_world_t * world = state->world;

    ecs_progress(world, 0);


    const sg_pass_action pass_action = {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sgl_draw();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void* user_data) {
    sgl_shutdown();
    sg_shutdown();
}

void event(const sapp_event* e) {

}



void glid_init(viz_state_t * state)
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

