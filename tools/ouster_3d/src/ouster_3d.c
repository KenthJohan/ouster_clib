#include "sokol_app.h"
#include "sokol_gfx.h"

#include "sokol_debugtext.h"

#include "sokol_glue.h"
#include "sokol_log.h"

#include "argparse.h"

#include "draw_points.h"
#include "gcamera.h"

#include <ouster_clib.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef enum {
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

typedef struct {
	int keys[512];
	gcamera_state_t camera;
	draw_points_t draw_points;
	pthread_t thread;
	pthread_mutex_t lock;
	float dt;
	float w;
	float h;

	char const *metafile;
	ouster_meta_t meta;
} app_t;

// helper function to print the help/status text
void print_status_text(app_t *app)
{
	sdtx_canvas(app->w * 0.5f, app->h * 0.5f);
	sdtx_origin(1.0f, 2.0f);
	sdtx_color3f(1.0f, 1.0f, 1.0f);
	sdtx_printf("FPS: %f", 1.0f / app->dt);
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

void gcamera_controller(gcamera_state_t *camera, int keys[])
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
	// printf("moving %f %f %f\n", V3_ARG(camera->moving));
	// printf("looking %f %f %f\n", V3_ARG(camera->looking));
}

static void frame_cb(app_t *app)
{
	app->dt = sapp_frame_duration();
	app->w = sapp_widthf();
	app->h = sapp_heightf();

	gcamera_controller(&app->camera, app->keys);
	gcamera_update(&app->camera, app->dt, app->w, app->h);

	sg_pass_action action = (sg_pass_action){.colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.2f, 0.4f, 1.0f}}};

	print_status_text(app);

	sg_begin_default_passf(&action, app->w, app->h);
	pthread_mutex_lock(&app->lock);
	draw_points_pass(&app->draw_points, &app->camera.vp);
	pthread_mutex_unlock(&app->lock);
	sdtx_draw();
	sg_end_pass();

	/*
	sg_begin_default_passf(&action, w, h);
	sdtx_draw();
	sg_end_pass();
	*/

	sg_commit();
}

void event_cb(const sapp_event *evt, app_t *app)
{
	int *keys = app->keys;

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
		assert(evt->key_code < 512);
		keys[evt->key_code] = 0;
		break;
	case SAPP_EVENTTYPE_KEY_DOWN:
		assert(evt->key_code < 512);
		keys[evt->key_code] = 1;
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


int convert(vertex_t * v, double * xyz, int n, float thres)
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
		v->w = 0.03f;
		j++;
		v++;
	}
	return j;
}

void *rec(app_t *app)
{
	ouster_meta_t *meta = &app->meta;

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, meta);

	int socks[2];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(7502, OUSTER_DEFAULT_RCVBUF_SIZE);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(7503, OUSTER_DEFAULT_RCVBUF_SIZE);

	ouster_lidar_t lidar = {0};

	ouster_lut_t lut = {0};
	ouster_lut_init(&lut, meta);
	double *xyz = calloc(1, lut.w * lut.h * sizeof(double) * 3);

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = ouster_net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char buf[OUSTER_NET_UDP_MAX_SIZE];
			int64_t n = ouster_net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			if (n == meta->lidar_packet_size) {
				ouster_lidar_get_fields(&lidar, meta, buf, fields, FIELD_COUNT);
				if (lidar.last_mid == meta->mid1) {
					ouster_lut_cartesian_f64(&lut, fields[FIELD_RANGE].data, xyz, sizeof(double)*3);
					ouster_field_zero(fields, FIELD_COUNT);
					printf("frame=%i, mid_loss=%i\n", lidar.frame_id, lidar.mid_loss);

					pthread_mutex_lock(&app->lock);
					int n = MIN(lut.w*lut.h, app->draw_points.vertices_cap);

					int j = convert(app->draw_points.vertices, xyz, n, 0.1);
					app->draw_points.vertices_count = j;
					pthread_mutex_unlock(&app->lock);

				}
			} else {
				printf("Bytes received (%ji) does not match lidar_packet_size (%ji)\n", (intmax_t)n, (intmax_t)app->meta.lidar_packet_size);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			// char buf[OUSTER_NET_UDP_MAX_SIZE];
			// int64_t n = ouster_net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
		}
	}

	return NULL;
}

static const char *const usages[] = {
    "ouster_3d [options] [[--] args]",
    "ouster_3d [options]",
    NULL,
};

sapp_desc sokol_main(int argc, char *argv[])
{
	ouster_os_set_api_defaults();
	ouster_fs_pwd();

	app_t *app = calloc(1, sizeof(app_t));
	gcamera_init(&app->camera);

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
