
#include <ouster_clib.h>

#include "sokol_app.h"
#include "sokol_gfx.h"

#include "sokol_debugtext.h"

#include "sokol_glue.h"
#include "sokol_log.h"

#include "argparse.h"


#include "draw_points.h"
#include "gcamera.h"
#include "save_csv.h"
#include "app.h"
#include "rxlidar.h"



#include <assert.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// helper function to print the help/status text
void print_status_text(app_t *app)
{
	sdtx_canvas(app->w * 0.5f, app->h * 0.5f);
	sdtx_origin(0.1f, 0.1f);
	sdtx_color3f(1.0f, 0.0f, 0.0f);
	sdtx_printf("FPS: [%f] ", 1.0f / app->dt);
	//sdtx_origin(1.0f, 3.0f);
	sdtx_printf("Pos: [%+5.2f %+5.2f %+5.2f]", app->camera.pos[0], app->camera.pos[1], app->camera.pos[2]);
}

static void init_cb(app_t *app)
{

	sg_setup(&(sg_desc){
	    .context = sapp_sgcontext(),
	    .logger.func = slog_func,
	});
	// setup sokol-debugtext
	sdtx_setup(&(sdtx_desc_t){
	    .fonts[0] = sdtx_font_z1013(),
	    .logger.func = slog_func,
	});

	draw_points_init(&app->draw_points);
}

void gcamera_controller(gcamera_state_t *camera, uint8_t keys[])
{
	camera->looking[0] = keys[SAPP_KEYCODE_UP] - keys[SAPP_KEYCODE_DOWN];
	camera->looking[1] = keys[SAPP_KEYCODE_RIGHT] - keys[SAPP_KEYCODE_LEFT];
	camera->looking[2] = keys[SAPP_KEYCODE_E] - keys[SAPP_KEYCODE_Q];
	camera->moving[0] = keys[SAPP_KEYCODE_A] - keys[SAPP_KEYCODE_D];
	camera->moving[1] = keys[SAPP_KEYCODE_LEFT_CONTROL] - keys[SAPP_KEYCODE_SPACE];
	camera->moving[2] = keys[SAPP_KEYCODE_W] - keys[SAPP_KEYCODE_S];
	camera->fov = keys[SAPP_KEYCODE_KP_0] ? 45 : camera->fov;
	camera->fov -= keys[SAPP_KEYCODE_KP_1];
	camera->fov += keys[SAPP_KEYCODE_KP_2];

	camera->moving[0] *= 0.10f;
	camera->moving[1] *= 0.10f;
	camera->moving[2] *= 0.10f;
	// printf("moving %f %f %f\n", V3_ARG(camera->moving));
	// printf("looking %f %f %f\n", V3_ARG(camera->looking));
}


int convert(vertex_t * v, double * xyz, int n, float thres, float radius)
{
	int j = 0;
	for(int i = 0; i < n; ++i, xyz += 3)
	{
		double l2 = V3_DOT(xyz, xyz);
		if(l2 < (thres*thres)){continue;}
		v->color = 0XFFFFFFFF;
		v->x = xyz[0];
		v->y = xyz[1];
		v->z = xyz[2];
		v->w = radius;
		j++;
		v++;
	}
	return j;
}


static void frame_cb(app_t *app)
{
	app->dt = sapp_frame_duration();
	app->w = sapp_widthf();
	app->h = sapp_heightf();

	gcamera_controller(&app->camera, app->keys);
	gcamera_update(&app->camera, app->dt, app->w, app->h);
	app->gui_point_radius += (app->keys[SAPP_KEYCODE_PAGE_UP] - app->keys[SAPP_KEYCODE_PAGE_DOWN]) * 0.1f;

	//printf("SAPP_KEYCODE_P %i\n", app->keys[SAPP_KEYCODE_P]);


	sg_pass_action action = (sg_pass_action){.colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.2f, 0.4f, 1.0f}}};

	print_status_text(app);

	sg_begin_default_passf(&action, app->w, app->h);


	if (app->keys[SAPP_KEYCODE_PAUSE] & KEY_PRESSED) {
		app->pause = !app->pause;
		printf(app->pause ? "Pause\n" : "Resume\n");
	}

	if (app->pause == 0)
	{
		pthread_mutex_lock(&app->lock);
		int n = MIN(app->points_count, app->draw_points.vertices_cap);
		int j = convert(app->draw_points.vertices, app->points_xyz, n, 0.1f, app->gui_point_radius);
		app->draw_points.vertices_count = j;
		pthread_mutex_unlock(&app->lock);
	}


	draw_points_pass(&app->draw_points, &app->camera.vp);

	if (app->keys[SAPP_KEYCODE_C] & KEY_PRESSED) {
		pthread_mutex_lock(&app->lock);
		printf("savecsv\n");
		save_csv(app->draw_points.vertices, app->draw_points.vertices_count);
		pthread_mutex_unlock(&app->lock);
	}


	sdtx_draw();
	sg_end_pass();

	/*
	sg_begin_default_passf(&action, w, h);
	sdtx_draw();
	sg_end_pass();
	*/

	sg_commit();


	for(int i = 0; i < 512; ++i) {
		app->keys[i] &= ~KEY_PRESSED;
	}
}

void event_cb(const sapp_event *evt, app_t *app)
{
	uint8_t *keys = app->keys;
	sapp_keycode k = evt->key_code;

	switch (evt->type) {
	case SAPP_EVENTTYPE_MOUSE_DOWN:
		if (evt->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
		}

		if (evt->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
		}
		break;
	case SAPP_EVENTTYPE_MOUSE_UP:
		if (evt->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
		}

		if (evt->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
		}
		break;
	case SAPP_EVENTTYPE_MOUSE_SCROLL:
		break;
	case SAPP_EVENTTYPE_KEY_UP:
		assert(k < 512);
		keys[k] = 0;
		break;
	case SAPP_EVENTTYPE_KEY_DOWN:
		assert(k < 512);
		//printf("(keys[k] & KEY_PRESS): %i\n", (keys[k] & KEY_PRESS));
		keys[k] |= (keys[k] & KEY_PRESS) ? 0 : KEY_PRESSED;
		keys[k] |= KEY_PRESS;
		break;
	case SAPP_EVENTTYPE_RESIZED: {
		break;
	}
	default:
		break;
	}
}

static void cleanup_cb(app_t *app)
{
	sdtx_shutdown();
	sg_shutdown();
}




static const char *const usages[] = {
    "ouster_3d [options] [[--] args]",
    "ouster_3d [options]",
    NULL,
};

sapp_desc sokol_main(int argc, char *argv[])
{
	ouster_os_set_api_defaults();
	ouster_log_set_level(0);
	ouster_fs_pwd();

	app_t *app = calloc(1, sizeof(app_t));
	app->gui_point_radius = 10.0f;
	gcamera_init(&app->camera);
	app->camera.pos[2] = -2.0f;

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('m', "metafile", &app->metafile, "Metafile path", NULL, 0, 0),
	    OPT_END(),
	};

	struct argparse argparse = {0};
	argparse_init(&argparse, options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, (char const **)argv);

	if (app->metafile == NULL) {
		argparse_usage(&argparse);
		exit(-1);
	}

	{
		char *content = ouster_fs_readfile(app->metafile);
		if (content == NULL) {
			exit(-1);
		}
		ouster_meta_parse(content, &app->meta);
		free(content);
		printf("Column window: %i %i\n", app->meta.mid0, app->meta.mid1);
	}

	int iret1 = pthread_create(&app->thread, NULL, (void *)rec, (void *)app);
	if (iret1 != 0) {
		printf("pthread_create error\n");
		exit(-1);
	}

	return (sapp_desc){
	    .user_data = app,
	    .init_userdata_cb = (void (*)(void *))init_cb,
	    .frame_userdata_cb = (void (*)(void *))frame_cb,
	    .cleanup_userdata_cb = (void (*)(void *))cleanup_cb,
	    .event_userdata_cb = (void (*)(const sapp_event *, void *))event_cb,
	    .width = 800,
	    .height = 600,
	    .sample_count = 4,
	    .window_title = "Primitive Types",
	    .icon.sokol_default = true,
	    .logger.func = slog_func,
	};
}
