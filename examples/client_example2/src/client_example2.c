#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ouster_client2/client.h"
#include "ouster_client2/lidar_header.h"
#include "ouster_client2/lidar_column.h"
#include "ouster_client2/net.h"
#include "ouster_client2/log.h"
#include "ouster_client2/types.h"
#include "ouster_client2/lidar_px.h"




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



typedef struct
{
    ouster_frame_id_t frame_id;
} ouster_complete_t;







int main(int argc, char* argv[])
{

    int socks[2];
    socks[SOCK_INDEX_LIDAR] = ouster_client_create_lidar_udp_socket("52985");
    socks[SOCK_INDEX_IMU] = ouster_client_create_imu_udp_socket("40140");

    ouster_mat_t mat;
    mat.esize = 4;
    mat.stride = mat.esize * 16;
    mat.data = malloc(mat.stride * 1024);


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
            ouster_log("%-10s %5ji:  ", "SOCK_LIDAR", (intmax_t)n);
            ouster_lidar_header_t header;
            ouster_lidar_header_get(buf, &header);
            ouster_lidar_header_log(&header);

            /*
            if(header.frame_id == -1)
            {
                printf("expecting to start batching a new scan!\n");
            }
            */

            //uint16_t frame_id = get_frame_id(buf + nth_col(packet_header_size, col_size, 0));
            
            char * colbuf = buf + OUSTER_PACKET_HEADER_SIZE;
            for(int icol = 0; icol < OUSTER_COLUMS_PER_PACKET; icol++, colbuf += OUSTER_COL_SIZE)
            {
                ouster_column_t column;
                ouster_column_get(colbuf, &column);
                ouster_column_log(&column);

                char * pxbuf = colbuf + OUSTER_COLUMN_HEADER_SIZE;

                ouster_pxcpy(mat.data + icol * mat.stride, mat.esize, pxbuf, OUSTER_CHANNEL_DATA_SIZE, OUSTER_PIXELS_PER_COLUMN, mat.esize);
            }
        }


        if(a & (1 << SOCK_INDEX_IMU))
        {
            char buf[1024*256];
            int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
            ouster_log("%-10s %5ji:  ", "SOCK_IMU", (intmax_t)n);
            ouster_log("\n");
        }
    }


    return 0;
}
