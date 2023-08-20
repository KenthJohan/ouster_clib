#include <stdio.h>

#include "ouster_client2/client.h"


int main(int argc, char* argv[])
{
    ouster_client_t client;
    ouster_client_init(&client, "192.168.1.137");
    return 0;
}
