#include "ouster_client2/client_config.h"
#include "ouster_client2/log.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int sock_tcp;
    char * buffer;
    int buffer_count;
    int buffer_cap;
} ouster_client_t;


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



void client_config_init(char const * host)
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL * curl = curl_easy_init();
    ouster_client_t * client = calloc(sizeof(ouster_client_t),1);
    client->buffer_cap = 1024;
    client->buffer = calloc(1, client->buffer_cap);
    //client->sock_tcp = ouster_create_imu_tcp_socket(host);
    get(client, curl, host, "api/v1/sensor/cmd/set_udp_dest_auto");
    free(client);

    //ouster_client_t client = {0};
    //ouster_client_init(&client, "192.168.1.137");
    //get(client, curl, host, "api/v1/sensor/cmd/set_udp_dest_auto");
}