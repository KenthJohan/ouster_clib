#include "ouster_client2/client.h"
#include "log.h"
#include "net.h"

#include <stdlib.h>






void ouster_client_init(ouster_client_t * client, char const * host)
{
    net_sock_desc_t desc;
    desc.flags = NET_FLAGS_UDP | NET_FLAGS_NONBLOCK | NET_FLAGS_REUSE | NET_FLAGS_BIND;
    desc.hint_name = NULL;
    desc.hint_service = "0";
    net_create(&desc);
}

