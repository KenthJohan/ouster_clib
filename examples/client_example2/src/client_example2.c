#include <stdio.h>
#include <pthread.h>

#include "ouster_client2/client.h"
#include "ouster_client2/lidar_header.h"
#include "ouster_client2/lidar_column.h"
#include "ouster_client2/net.h"


typedef struct {
    pthread_t reader_thread;
    pthread_t process_thread;
    int reader_fn_count;
    int process_fn_count;
    pthread_mutex_t mutex;
    pthread_cond_t c_cons;
    pthread_cond_t c_prod;
} app_args;


void reader_fn(void * userptr)
{
    app_args * a = (app_args *)userptr;
    int socks[2];
    socks[0] = ouster_client_create_lidar_udp_socket("50876");
    socks[1] = ouster_client_create_imu_udp_socket("38278");

    ouster_pf_t pf;
    pf.packet_header_size = 0;
    pf.col_header_size = 16;
    pf.channel_data_size = 12;
    pf.col_footer_size = 4;
    pf.packet_footer_size = 0;
    pf.col_size = 212;
    pf.lidar_packet_size = 3392;
    pf.timestamp_offset = 0;
    pf.measurement_id_offset = 8;
    pf.status_offset = 208;

    while(1)
    //for(int i = 0; i < 100; ++i)
    {
        a->reader_fn_count++;
        char buf[1024*256];
        uint64_t a = net_select(socks, 2, 1);
        //printf("net_select %jx\n", (uintmax_t)a);
        if(a & 0x1)
        {
            int64_t n = net_read(socks[0], buf, sizeof(buf));
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
            
            for(int icol = 0; icol < 16; icol++)
            {
                ouster_column_t column;
                ouster_column_get(buf, icol, &pf, &column);
                //ouster_column_log(&column);
            }
        }
        if(a & 0x2)
        {
            int64_t n = net_read(socks[1], buf, sizeof(buf));
            printf("Sock2 %ji\n", (intmax_t)n);
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
