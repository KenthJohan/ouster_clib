#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ouster_clib/lidar.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/ouster_fs.h>
#include <ouster_clib/ouster_log.h>
#include <ouster_clib/ouster_net.h>
#include <ouster_clib/sock.h>
#include <ouster_clib/types.h>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef enum {
	SOCK_INDEX_LIDAR,
	SOCK_INDEX_IMU,
	SOCK_INDEX_COUNT
} sock_index_t;

int pixsize_to_cv_type(int size)
{
	switch (size) {
	case 1:
		return CV_8U;
	case 2:
		return CV_16U;
	case 4:
		return CV_32S;
	}
	return 0;
}

cv::Mat ouster_get_cvmat(ouster_field_t *field)
{
	int cols = field->cols;
	int rows = field->rows;
	int depth = field->depth;
	cv::Mat m(rows, cols, pixsize_to_cv_type(depth), field->data);
	return m;
}

#define FIELD_COUNT 4

int main(int argc, char *argv[])
{
	{
		char cwd[1024] = {0};
		getcwd(cwd, sizeof(cwd));
		printf("Current working dir: %s\n", cwd);
	}

	ouster_meta_t meta = {0};
	if (argc > 1) {
		char const *content = fs_readfile(argv[1]);
		if (content == NULL) {
			return 0;
		}
		ouster_meta_parse(content, &meta);
		free((void *)content);
	} else {
		printf("Missing input file argument");
		return 0;
	}

	printf("Column window: %i %i\n", meta.mid0, meta.mid1);

	int socks[SOCK_INDEX_COUNT];
	socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar(meta.udp_port_lidar);
	socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu(meta.udp_port_imu);

	ouster_field_t fields[FIELD_COUNT] = {
	    {.quantity = OUSTER_QUANTITY_RANGE, .depth = 4},
	    {.quantity = OUSTER_QUANTITY_REFLECTIVITY, .depth = 4},
	    {.quantity = OUSTER_QUANTITY_SIGNAL, .depth = 4},
	    {.quantity = OUSTER_QUANTITY_NEAR_IR, .depth = 4}};

	ouster_field_init(fields, FIELD_COUNT, &meta);
	cv::Mat mat_f0 = ouster_get_cvmat(fields + 0);
	cv::Mat mat_f1 = ouster_get_cvmat(fields + 1);
	cv::Mat mat_f2 = ouster_get_cvmat(fields + 2);
	cv::Mat mat_f3 = ouster_get_cvmat(fields + 3);

	{
		int cols = meta.columns_per_frame;
		int rows = meta.pixels_per_column;
		int colsf = 5;
		int rowsf = 5;
		cv::namedWindow("RANGE", cv::WINDOW_FREERATIO);
		cv::namedWindow("REFLECTIVITY", cv::WINDOW_FREERATIO);
		cv::namedWindow("SIGNAL", cv::WINDOW_FREERATIO);
		cv::namedWindow("NEAR_IR", cv::WINDOW_FREERATIO);
		cv::resizeWindow("RANGE", cols * colsf, rows * rowsf);
		cv::resizeWindow("REFLECTIVITY", cols * colsf, rows * rowsf);
		cv::resizeWindow("SIGNAL", cols * colsf, rows * rowsf);
		cv::resizeWindow("RANGE", cols * colsf, rows * rowsf);
	}

	ouster_lidar_t lidar = {0};

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
			// ouster_log("%-10s %5ji %5ji:  \n", "SOCK_LIDAR", (intmax_t)n, meta.lidar_packet_size);
			if (n != meta.lidar_packet_size) {
				// ouster_log("%-10s %5ji of %5ji:  \n", "SOCK_LIDAR", (intmax_t)n, meta.lidar_packet_size);
			}
			ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
			ouster_log("mid_loss %i\n", lidar.mid_loss);
			if (lidar.last_mid == meta.mid1) {
				ouster_field_destagger(fields, FIELD_COUNT, &meta);
				cv::Mat mat_f0_show;
				cv::Mat mat_f1_show;
				cv::Mat mat_f2_show;
				cv::Mat mat_f3_show;
				cv::normalize(mat_f0, mat_f0_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
				cv::normalize(mat_f1, mat_f1_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
				cv::normalize(mat_f2, mat_f2_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
				cv::normalize(mat_f3, mat_f3_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);

				cv::imshow("RANGE", mat_f0_show);
				cv::imshow("REFLECTIVITY", mat_f1_show);
				cv::imshow("SIGNAL", mat_f2_show);
				cv::imshow("NEAR_IR", mat_f3_show);

				// printf("mat = %i of %i\n", mat.num_valid_pixels, mat.dim[1] * mat.dim[2]);
				ouster_field_zero(fields, FIELD_COUNT);

				int key = cv::pollKey();
				if (key == 27) {
					break;
				}
			}
		}

		if (a & (1 << SOCK_INDEX_IMU)) {
			char buf[NET_UDP_MAX_SIZE];
			int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
			// ouster_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
		}

		// int key = cv::waitKey(1);
	}

	return 0;
}
