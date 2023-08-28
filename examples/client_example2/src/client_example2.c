#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ouster_client2/client.h"
#include "ouster_client2/net.h"
#include "ouster_client2/log.h"
#include "ouster_client2/types.h"
#include "ouster_client2/lidar_context.h"
#include "ouster_client2/mat.h"
#include "ouster_client2/os_file.h"
#include "ouster_client2/meta.h"




#define OUSTER_PACKET_HEADER_SIZE 32
#define OUSTER_COLUMN_HEADER_SIZE 12
#define OUSTER_CHANNEL_DATA_SIZE 12
#define OUSTER_COLUMS_PER_PACKET 16
#define OUSTER_PIXELS_PER_COLUMN 16
#define OUSTER_COLUMN_HEADER_SIZE 12
#define OUSTER_COL_SIZE ((OUSTER_PIXELS_PER_COLUMN*OUSTER_CHANNEL_DATA_SIZE)+OUSTER_COLUMN_HEADER_SIZE)





typedef enum 
{
    SOCK_INDEX_LIDAR,
    SOCK_INDEX_IMU,
    SOCK_INDEX_COUNT
} sock_index_t;








int main(int argc, char* argv[])
{
    {
        char cwd[1024] = {0};
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
    }

    char const * meta = ouster_os_file_read("./meta.json");
    ouster_meta_parse(meta);
    //printf("Meta:\n%s\n", meta);
    return 0;

    int socks[2];
    socks[SOCK_INDEX_LIDAR] = ouster_client_create_lidar_udp_socket("7502");
    socks[SOCK_INDEX_IMU] = ouster_client_create_imu_udp_socket("7503");

    ouster_mat4_t mat = {.dim = {4, 16, 1024, 1}};
    ouster_mat4_init(&mat);

    lidar_context_t lidctx =
    {
        .packet_header_size = OUSTER_PACKET_HEADER_SIZE,
        .columns_per_packet = OUSTER_COLUMS_PER_PACKET,
        .col_size = OUSTER_COL_SIZE,
        .column_header_size = OUSTER_COLUMN_HEADER_SIZE,
        .channel_data_size = OUSTER_CHANNEL_DATA_SIZE,
        .pixels_per_column = OUSTER_PIXELS_PER_COLUMN,
    };

    //for(int i = 0; i < 100; ++i)
    while(1)
    {
        int timeout_seconds = 1;
        uint64_t a = net_select(socks, SOCK_INDEX_COUNT, timeout_seconds);

        if(a == 0)
        {
            ouster_log("Timeout\n");
        }


        if(a & (1 << SOCK_INDEX_LIDAR))
        {
            char buf[1024*10];
            int64_t n = net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
            //ouster_log("%-10s %5ji:  \n", "SOCK_LIDAR", (intmax_t)n);
            lidar_context_get_range(&lidctx, buf, &mat);
            if(lidctx.mid_last == lidctx.mid_max)
            {
                //printf("mat = %i of %i\n", mat.num_valid_pixels, mat.dim[1] * mat.dim[2]);
                ouster_mat4_zero(&mat);
            }
        }


        if(a & (1 << SOCK_INDEX_IMU))
        {
            char buf[1024*256];
            int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
            //ouster_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
        }
    }


    return 0;
}
