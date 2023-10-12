#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib/client.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/lidar_column.h>
#include <ouster_clib/lidar_header.h>
#include <ouster_clib/lut.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/ouster_assert.h>
#include <ouster_clib/ouster_fs.h>
#include <ouster_clib/ouster_log.h>
#include <ouster_clib/ouster_net.h>
#include <ouster_clib/sock.h>
#include <ouster_clib/types.h>

#include "draw.h"
#include "tigr.h"

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

void print_help(int argc, char *argv[])
{
	printf("Hello welcome to %s!!\n", argv[0]);
	printf("This tool takes snapshots from LiDAR sensor and saves it as PNG image.\n");
	printf("To use this tool you will have to provide the meta file that correspond to the LiDAR sensor configuration\n");
	printf("\t$ %s <%s> <%s> <%s>\n", argv[0], "meta.json", "mode", "output_filename");
	printf("\t$ %s <%s> %s %s\n", argv[0], "meta.json", "raw", "raw.png");
	printf("\t$ %s <%s> %s %s\n", argv[0], "meta.json", "destagger", "destaggered.png");
}

void cpy(uint32_t *src, Tigr *bmp, int w, int h)
{
	remap(src, src, w * h);
	TPixel *td = bmp->pix;
	for (int y = 0; y < h; ++y, td += w, src += w) {
		for (int x = 0; x < w; ++x) {
			uint32_t a = src[x];
			td[x].a = 0xFF;
			td[x].r = a;
			td[x].g = a;
			td[x].b = a;
		}
	}
}

typedef struct
{
	monitor_mode_t mode;
	char const *metafile;
	ouster_meta_t meta;
	pthread_mutex_t lock;
	Tigr *bmp;
} app_t;

void *rec(void *ptr)
{
	app_t *app = ptr;
	ouster_meta_t *meta = &app->meta;

	ouster_field_t fields[FIELD_COUNT] = {
	    [FIELD_RANGE] = {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, meta);

	int socks[2];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(7502);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(7503);

	ouster_lidar_t lidar = {0};

	int w = app->meta.midw;
	int h = app->meta.pixels_per_column;

	while (1) {
		int timeout_sec = 1;
		int timeout_usec = 0;
		uint64_t a = net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

		if (a == 0) {
			ouster_log("Timeout\n");
		}

		if (a & (1 << SOCK_INDEX_LIDAR)) {
			char buf[NET_UDP_MAX_SIZE];
			int64_t n = net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
			ouster_lidar_get_fields(&lidar, meta, buf, fields, FIELD_COUNT);
			if (lidar.last_mid == meta->mid1) {
				if (app->mode == SNAPSHOT_MODE_RAW) {
					printf("save_png SNAPSHOT_MODE_RAW\n");
					// image_saver_save(&saver, fields[FIELD_RANGE].data);
				}
				if (app->mode == SNAPSHOT_MODE_DESTAGGER) {
					printf("save_png SNAPSHOT_MODE_DESTAGGER\n");
					ouster_field_destagger(fields, FIELD_COUNT, meta);
					cpy(fields[FIELD_RANGE].data, app->bmp, w, h);
					// image_saver_save(&saver, fields[FIELD_RANGE].data);
				}
				ouster_field_zero(fields, FIELD_COUNT);
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			// char buf[NET_UDP_MAX_SIZE];
			// int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
		}
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	printf("===================================================================\n");
	fs_pwd();

	if (argc != 3) {
		print_help(argc, argv);
		return 0;
	}

	app_t app = {
	    .lock = PTHREAD_MUTEX_INITIALIZER};
	app.metafile = argv[1];
	app.mode = get_mode(argv[2]);

	if (app.mode == SNAPSHOT_MODE_UNKNOWN) {
		printf("The mode <%s> does not exist.\n", argv[2]);
		print_help(argc, argv);
		return 0;
	}

	{
		char *content = fs_readfile(app.metafile);
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

	Tigr *screen = tigrWindow(w, h, "ouster_view", TIGR_AUTO);
	while (!tigrClosed(screen)) {
		tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));
		pthread_mutex_lock(&app.lock);
		tigrBlit(screen, app.bmp, 0, 0, 0, 0, w, h);
		pthread_mutex_unlock(&app.lock);
		tigrUpdate(screen);
	}
	tigrFree(screen);

	return 0;
}
