#include "net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include "log.h"

void print_1(struct addrinfo * ai)
{
    void * a = NULL;
    switch (ai->ai_family)
    {
    case AF_INET:
        a = &(((struct sockaddr_in*)ai->ai_addr)->sin_addr);
        break;
    case AF_INET6:
        a = &(((struct sockaddr_in6*)ai->ai_addr)->sin6_addr);
        break;
    }
    char buf[INET6_ADDRSTRLEN];
    inet_ntop(ai->ai_family, a, buf, INET6_ADDRSTRLEN);
    printf("%s\n", buf);
}


int net_create(net_sock_desc_t * desc)
{

    struct addrinfo * info_start;
    struct addrinfo * ai;
    {
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        if(desc->flags & NET_FLAGS_IPV4)
        {
            hints.ai_family = AF_INET;
        }
        if(desc->flags & NET_FLAGS_IPV6)
        {
            hints.ai_family = AF_INET6;
        }
        if(desc->flags & NET_FLAGS_UDP)
        {
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_flags = AI_PASSIVE;
        }
        int ret = getaddrinfo(desc->hint_name, desc->hint_service, &hints, &info_start);
        if (ret != 0)
        {
            ouster_log("udp getaddrinfo(): %s\n", gai_strerror(ret));
            return -1;
        }
        if (info_start == NULL)
        {
            ouster_log("udp getaddrinfo(): empty result\n");
            return -1;
        }
    }

    for (ai = info_start; ai != NULL; ai = ai->ai_next)
    {
        print_1(ai);
        ouster_log("getaddrinfo %i\n", ai->ai_family);
    }
    ai = info_start;

    int s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if(s < 0)
    {
        ouster_log("udp socket(): error\n");
        return -1;
    }

    int rc;
    if(desc->flags & NET_FLAGS_IPV6ONLY)
    {
        int off = 0;
        rc = setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));
        if(rc)
        {
            ouster_log("setsockopt(): error\n");
            return rc;
        }
    }

    if(desc->flags & NET_FLAGS_REUSE)
    {
        int option = 1;
        rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
        if(rc)
        {
            ouster_log("setsockopt(): error\n");
            return rc;
        }
    }

    if(desc->flags & NET_FLAGS_BIND)
    {
        rc = bind(s, ai->ai_addr, (socklen_t)ai->ai_addrlen);
        if(rc)
        {
            ouster_log("bind(): error\n");
            return -1;
        }
    }



    if(desc->flags & NET_FLAGS_NONBLOCK)
    {
        int flags = fcntl(s, F_GETFL, 0);
        if(flags == -1)
        {
            ouster_log("fcntl(): error\n");
            return -1;
        }
        rc = fcntl(s, F_SETFL, flags | O_NONBLOCK);
        if(rc == -1)
        {
            ouster_log("fcntl(): error\n");
            return -1;
        }
    }
    if(desc->rcvbuf)
    {
        rc = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&desc->rcvbuf, sizeof(desc->rcvbuf));
        if(rc)
        {
            ouster_log("setsockopt(): error\n");
            return rc;
        }
    }


    return 0;
}