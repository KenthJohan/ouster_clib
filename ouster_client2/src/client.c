#include "ouster_client2/client.h"
#include "log.h"
#include "net.h"
#include <curl/curl.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



size_t write_memory_callback(void* contents, size_t element_size, size_t elements_count, void* user_pointer)
{
    size_t size_increment = element_size * elements_count;
    ouster_client_t * client = user_pointer;
    int new_size = client->buffer_count + size_increment;
    if(new_size > client->buffer_cap)
    {
        client->buffer = realloc(client->buffer, new_size);
        if(client->buffer == NULL)
        {
            ouster_log("realloc(): error\n");
            return 0;
        }
        client->buffer_cap = new_size;
    }
    printf("contents:\n");
    fwrite(contents, size_increment, 1, stdout);
    printf("\n");
    memcpy(client->buffer + client->buffer_count, contents, size_increment);
    client->buffer_count += size_increment;
    return size_increment;
}

//http://192.168.1.137/api/v1/sensor/cmd/set_udp_dest_auto
//http://192.168.1.137/api/v1/sensor/cmd/set_udp_dest_auto
void get(ouster_client_t * client, CURL* handle, char const * host, char const * url)
{
    char buf[1024];
    snprintf(buf, 1024, "http://%s/%s", host, url);
    ouster_log("get request: %s\n", buf);
    curl_easy_setopt(handle, CURLOPT_URL, buf);
    curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, client);
    CURLcode res = curl_easy_perform(handle);
    if (res == CURLE_SEND_ERROR) {
        // Specific versions of curl does't play well with the sensor http
        // server. When CURLE_SEND_ERROR happens for the first time silently
        // re-attempting the http request resolves the problem.
        res = curl_easy_perform(handle);
    }

    if (res != CURLE_OK)
    {
        ouster_log("curl_easy_perform() error\n");
    }
    long http_code = 0;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200)
    {
        ouster_log("http_code error: %i\n", http_code);
    }
}

int nth_col(int packet_header_size, int col_size, int n)
{
    return packet_header_size + (n * col_size);
}

uint16_t get_frame_id(uint8_t const * buf)
{
    uint16_t res;
    memcpy(&res, buf + 10, sizeof(uint16_t));
}

uint64_t get_col_timestamp(uint8_t const* buf)
{
    uint64_t res;
    memcpy(&res, buf, sizeof(uint64_t));
    return res;
}

uint16_t get_col_measurement_id(uint8_t const* buf)
{
    uint16_t res;
    memcpy(&res, buf, sizeof(uint16_t));
    return res;
}


void ouster_client_init(ouster_client_t * client, char const * host)
{
    {
        net_sock_desc_t desc = {0};
        desc.flags = NET_FLAGS_UDP | NET_FLAGS_NONBLOCK | NET_FLAGS_REUSE | NET_FLAGS_BIND;
        desc.hint_name = NULL;
        desc.rcvbuf_size = 256 * 1024;
        desc.hint_service = "46942";
        client->socks[0] = net_create(&desc);
        desc.hint_service = "43945";
        client->socks[1] = net_create(&desc);
        client->buffer_cap = 1024;
        client->buffer = calloc(1, client->buffer_cap);
    }

    {
        net_sock_desc_t desc = {0};
        desc.flags = NET_FLAGS_TCP | NET_FLAGS_CONNECT;
        desc.hint_name = host;
        desc.hint_service = "7501";
        desc.rcvtimeout_sec = 10;
        client->sock_tcp = net_create(&desc);
    }


    int p1 = net_get_port(client->socks[0]);
    int p2 = net_get_port(client->socks[1]);
    printf("p1p2 %i %i\n", p1, p2);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL * curl = curl_easy_init();
    //get(client, curl, host, "api/v1/sensor/cmd/set_udp_dest_auto");

    int col_size = 212;
    int packet_header_size = 0;
    int timestamp_offset = 0;
    int measurement_id_offset = 0;
    int col_header_size = 16;
    int lidar_packet_size = 3392;
    int status_offset = 208;
    while(1)
    {
        uint8_t buf[1024*256];
        uint64_t a = net_select(client->socks, 2, 1);
        //printf("net_select %jx\n", (uintmax_t)a);
        if(a & 0x1)
        {
            int64_t n = net_read(client->socks[0], (char*)buf, sizeof(buf));
            uint16_t frame_id = get_frame_id(buf + nth_col(packet_header_size, col_size, 0));
            printf("Sock1 %ji, frame_id=%ji =================\n", (intmax_t)n, (intmax_t)frame_id);
            for(int icol = 0; icol < 16; icol++)
            {
                uint64_t ts = get_col_timestamp(buf + nth_col(packet_header_size, col_size, icol));
                uint16_t id = get_col_measurement_id(buf + nth_col(packet_header_size, col_size, icol));
                printf("col%i: %ji %ji\n", icol, (intmax_t)id, (intmax_t)ts);
            }
        }
        if(a & 0x2)
        {
            int64_t n = net_read(client->socks[1], buf, sizeof(buf));
            printf("Sock2 %ji\n", (intmax_t)n);
        }
    }

}

