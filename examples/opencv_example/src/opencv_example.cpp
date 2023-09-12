#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ouster_clib/sock.h>
#include <ouster_clib/types.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/meta.h>


#include <platform/net.h>
#include <platform/log.h>
#include <platform/fs.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


typedef enum 
{
    SOCK_INDEX_LIDAR,
    SOCK_INDEX_IMU,
    SOCK_INDEX_COUNT
} sock_index_t;


int pixsize_to_cv_type(int size)
{
    switch (size)
    {
    case 1: return CV_8U;
    case 2: return CV_16U;
    case 4: return CV_32S;
    }
    return 0;
}

cv::Mat ouster_get_cvmat(ouster_field_t * field)
{
    cv::Mat m(field->rows, field->cols, pixsize_to_cv_type(field->depth), field->data);
    return m;
}



int main(int argc, char* argv[])
{
    {
        char cwd[1024] = {0};
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
    }

    ouster_meta_t meta = {0};
    if(argc > 1)
    {
        char const * content = fs_readfile(argv[1]);
        if(content == NULL){return 0;}
        ouster_meta_parse(content, &meta);
        free((void*)content);
    }
    else
    {
        printf("Missing input file argument");
        return 0;
    }

    printf("Column window: %i %i\n", meta.mid0, meta.mid1);

    int socks[SOCK_INDEX_COUNT];
    socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar("7502");
    socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_imu("7503");

    #define FIELD_COUNT 4
    ouster_field_t fields[FIELD_COUNT] = {
        {.quantity = OUSTER_QUANTITY_RANGE},
        {.quantity = OUSTER_QUANTITY_REFLECTIVITY},
        {.quantity = OUSTER_QUANTITY_SIGNAL},
        {.quantity = OUSTER_QUANTITY_NEAR_IR}
    };

    ouster_field_init(fields, FIELD_COUNT, &meta);

    cv::namedWindow("RANGE", cv::WINDOW_FREERATIO);
    cv::namedWindow("REFLECTIVITY", cv::WINDOW_FREERATIO);
    cv::namedWindow("SIGNAL", cv::WINDOW_FREERATIO);
    cv::namedWindow("NEAR_IR", cv::WINDOW_FREERATIO);
    cv::resizeWindow("RANGE", fields[0].cols*5, fields[0].rows*5);
    cv::resizeWindow("REFLECTIVITY", fields[1].cols*5, fields[1].rows*2);
    cv::resizeWindow("SIGNAL", fields[2].cols*5, fields[2].rows*2);
    cv::resizeWindow("RANGE", fields[3].cols*5, fields[3].rows*2);
    

    ouster_lidar_t lidar = {0};

    while(1)
    {
        
        int timeout_sec = 0;
        int timeout_usec = 1000*20;
        uint64_t a = net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

        if(a == 0)
        {
            //ouster_log("Timeout\n");
        }

        //https://static.ouster.dev/sdk-docs/reference/lidar-scan.html#staggering-and-destaggering

        if(a & (1 << SOCK_INDEX_LIDAR))
        {
            char buf[1024*1024];
            int64_t n = net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
            //platform_log("%-10s %5ji %5ji:  \n", "SOCK_LIDAR", (intmax_t)n, meta.lidar_packet_size);
            if(n != meta.lidar_packet_size)
            {
                //platform_log("%-10s %5ji of %5ji:  \n", "SOCK_LIDAR", (intmax_t)n, meta.lidar_packet_size);
            }
            ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
            platform_log("mid_loss %i\n", lidar.mid_loss);
            if(lidar.last_mid == meta.mid1)
            {

                ouster_field_destagger(fields, FIELD_COUNT, &meta);

                cv::Mat mat_f0 = ouster_get_cvmat(fields + 0);
                cv::Mat mat_f1 = ouster_get_cvmat(fields + 1);
                cv::Mat mat_f2 = ouster_get_cvmat(fields + 2);
                cv::Mat mat_f3 = ouster_get_cvmat(fields + 3);
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
                
                //printf("mat = %i of %i\n", mat.num_valid_pixels, mat.dim[1] * mat.dim[2]);
                ouster_field_zero(fields, FIELD_COUNT);


                int key = cv::pollKey();
                if(key == 27){break;}

            }
        }


        if(a & (1 << SOCK_INDEX_IMU))
        {
            char buf[1024*256];
            int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
            //platform_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
        }

        //int key = cv::waitKey(1);
    }


    return 0;
}
