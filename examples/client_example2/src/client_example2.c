#include <stdio.h>
#include <pthread.h>

#include "ouster_client2/client.h"
#include "ouster_client2/lidar_header.h"
#include "ouster_client2/lidar_column.h"
#include "ouster_client2/net.h"
#include "ouster_client2/log.h"








#define OUSTER_PACKET_HEADER_SIZE 32
#define OUSTER_COLUMN_HEADER_SIZE 12
#define OUSTER_CHANNEL_DATA_SIZE 12
#define OUSTER_COLUMS_PER_PACKET 16
#define OUSTER_PIXELS_PER_COLUMN 16
#define OUSTER_COL_SIZE ((OUSTER_PIXELS_PER_COLUMN*OUSTER_CHANNEL_DATA_SIZE)+OUSTER_COLUMN_HEADER_SIZE)







typedef struct {
    pthread_t reader_thread;
    pthread_t process_thread;
    int reader_fn_count;
    int process_fn_count;
    pthread_mutex_t mutex;
    pthread_cond_t c_cons;
    pthread_cond_t c_prod;
} app_args;


typedef enum 
{
    SOCK_INDEX_LIDAR,
    SOCK_INDEX_IMU,
    SOCK_INDEX_COUNT
} sock_index_t;


void * reader_fn(void * userptr)
{
    app_args * a = (app_args *)userptr;
    int socks[2];
    socks[SOCK_INDEX_LIDAR] = ouster_client_create_lidar_udp_socket("37346");
    socks[SOCK_INDEX_IMU] = ouster_client_create_imu_udp_socket("34608");

    //for(int i = 0; i < 100; ++i)
    while(1)
    {
        a->reader_fn_count++;

        int timeout_seconds = 1;
        uint64_t a = net_select(socks, SOCK_INDEX_COUNT, timeout_seconds);

        if(a == 0)
        {
            ouster_log("Timeout\n");
        }


        if(a & (1 << SOCK_INDEX_LIDAR))
        {
            char buf[1024*256];
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
            
            for(int icol = 0; icol < OUSTER_COLUMS_PER_PACKET; icol++)
            {
                ouster_column_t column;
                char * colbuf = buf + OUSTER_PACKET_HEADER_SIZE + OUSTER_COL_SIZE * icol;
                ouster_column_get(colbuf, icol, &column);
                ouster_column_log(&column);
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
}


int main(int argc, char* argv[])
{
    app_args a = 
    {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .c_cons = PTHREAD_COND_INITIALIZER,
        .c_prod = PTHREAD_COND_INITIALIZER
    };
    pthread_create(&a.reader_thread, NULL, reader_fn, &a);
    pthread_join(a.reader_thread, NULL);
    return 0;
}
