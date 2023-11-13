#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib.h>

#include "convert.h"
#include "tigr.h"
#include "tigr_mouse.h"

#include "argparse.h"

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

typedef enum {
	FIELD_RANGE,
	FIELD_COUNT
} field_t;

typedef enum {
	SNAPSHOT_MODE_UNKNOWN,
	SNAPSHOT_MODE_RAW,
	SNAPSHOT_MODE_DESTAGGER,
	MONITOR_MODE_COUNT
} monitor_mode_t;

mode_t get_mode(char const *str)
{
	if (strcmp(str, "raw") == 0) {
		return SNAPSHOT_MODE_RAW;
	}
	if (strcmp(str, "destagger") == 0) {
		return SNAPSHOT_MODE_DESTAGGER;
	}
	return SNAPSHOT_MODE_UNKNOWN;
}

static const char *const usages[] = {
    "ouster_view [options] [[--] args]",
    "ouster_view [options]",
    NULL,
};

typedef struct
{
	char const *metafile;
	char const *modestr;
	monitor_mode_t mode;
	ouster_meta_t meta;
	pthread_mutex_t lock;
	Tigr *bmp;
	tigr_mouse_t mouse;
} app_t;

void draw_mouse(Tigr *bmp, tigr_mouse_t *mouse, int w, int h, uint32_t *range)
{
	if (mouse->btn) {
		char buf[512];
		int i = mouse->y * w + mouse->x;
		if (i < 0) {
			return;
		}
		if (i >= (w * h)) {
			return;
		}
		// snprintf(buf, sizeof(buf), "%i %i", mouse->x, mouse->y);
		snprintf(buf, sizeof(buf), "%i", range[i]);
		tigrPrint(bmp, tfont, mouse->x + 5, mouse->y + 5, (TPixel){.r = 0x61, .g = 0x3C, .b = 0x66, .a = 0xFF}, buf);
		tigrPlot(bmp, mouse->x, mouse->y, (TPixel){.r = 0xFF, .g = 0x55, .b = 0x55, .a = 0xFF});
	}
}

void *rec(void *ptr)
{
	app_t *app = ptr;
	ouster_meta_t *meta = &app->meta;

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, meta);

	int socks[2];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(7502, OUSTER_DEFAULT_RCVBUF_SIZE);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(7503, OUSTER_DEFAULT_RCVBUF_SIZE);

	ouster_lidar_t lidar = {0};

	int w = app->meta.midw;
	int h = app->meta.pixels_per_column;

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
					if (app->mode == SNAPSHOT_MODE_RAW) {
					}
					if (app->mode == SNAPSHOT_MODE_DESTAGGER) {
						ouster_field_destagger(fields, FIELD_COUNT, meta);
					}

					pthread_mutex_lock(&app->lock);
					convert_u32_to_bmp(fields[FIELD_RANGE].data, app->bmp, w, h);
					draw_mouse(app->bmp, &app->mouse, w,h, fields[0].data);
					pthread_mutex_unlock(&app->lock);

					ouster_field_zero(fields, FIELD_COUNT);
					printf("frame=%i, mid_loss=%i\n", lidar.frame_id, lidar.mid_loss);
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

int main(int argc, char const *argv[])
{
	printf("ouster_view==========================\n");
	ouster_fs_pwd();

	app_t app = {
	    .lock = PTHREAD_MUTEX_INITIALIZER};

	struct argparse_option options[] = {
	    OPT_HELP(),
	    OPT_GROUP("Basic options"),
	    OPT_STRING('m', "metafile", &app.metafile, "The meta file that correspond to the LiDAR sensor configuration", NULL, 0, 0),
	    OPT_STRING('v', "view", &app.modestr, "View mode, raw, destagger", NULL, 0, 0),
	    OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, argv);

	if (app.metafile == NULL) {
		argparse_usage(&argparse);
		return -1;
	}

	if (app.modestr == NULL) {
		argparse_usage(&argparse);
		return -1;
	}

	app.mode = get_mode(app.modestr);

	if (app.mode == SNAPSHOT_MODE_UNKNOWN) {
		printf("The mode <%s> does not exist.\n", app.modestr);
		argparse_usage(&argparse);
		return 0;
	}

	{
		char *content = ouster_fs_readfile(app.metafile);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &app.meta);
		free(content);
		printf("Column window: %i %i\n", app.meta.mid0, app.meta.mid1);
	}

	int w = app.meta.midw;
	int h = app.meta.pixels_per_column;

	app.bmp = tigrBitmap(w, h);

	{
		pthread_t thread1;
		int rc = pthread_create(&thread1, NULL, rec, (void *)&app);
		ouster_assert(rc == 0, "");
	}

	int flags = 0;
	Tigr *screen = NULL;

again:
	if (screen) {
		tigrFree(screen);
	}
	screen = tigrWindow(w, h, "ouster_view", flags | TIGR_AUTO);
	while (!tigrClosed(screen)) {
		int c = tigrReadChar(screen);
		// printf("c: %i\n", c);
		switch (c) {
		case '1':
			flags &= ~(TIGR_2X | TIGR_3X | TIGR_4X);
			goto again;
		case '2':
			flags &= ~(TIGR_2X | TIGR_3X | TIGR_4X);
			flags |= TIGR_2X;
			goto again;
		case '3':
			flags &= ~(TIGR_2X | TIGR_3X | TIGR_4X);
			flags |= TIGR_3X;
			goto again;
		case '4':
			flags &= ~(TIGR_2X | TIGR_3X | TIGR_4X);
			flags |= TIGR_4X;
			goto again;

		default:
			break;
		}
		tigr_mouse_get(screen, &app.mouse);

		tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));
		pthread_mutex_lock(&app.lock);

		tigrBlit(screen, app.bmp, 0, 0, 0, 0, w, h);

		pthread_mutex_unlock(&app.lock);
		tigrUpdate(screen);
	}
	tigrFree(screen);

	return 0;
}
