#include "net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "log.h"

void inet_ntop_addrinfo(struct addrinfo * ai, char * buf, socklen_t len)
{
    void * addr = NULL;
    switch (ai->ai_family)
    {
    case AF_INET:
        addr = &(((struct sockaddr_in*)ai->ai_addr)->sin_addr);
        break;
    case AF_INET6:
        addr = &(((struct sockaddr_in6*)ai->ai_addr)->sin6_addr);
        break;
    }
    inet_ntop(ai->ai_family, addr, buf, len);
}


int net_create(net_sock_desc_t * desc)
{
    struct addrinfo * info = NULL;
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
        int ret = getaddrinfo(desc->hint_name, desc->hint_service, &hints, &info);
        if (ret != 0)
        {
            ouster_log("udp getaddrinfo(): %s\n", gai_strerror(ret));
            goto error;
        }
        if (info == NULL)
        {
            ouster_log("udp getaddrinfo(): empty result\n");
            goto error;
        }
    }

    for (struct addrinfo * ai = info; ai != NULL; ai = ai->ai_next)
    {
        char buf[INET6_ADDRSTRLEN];
        inet_ntop_addrinfo(ai, buf, INET6_ADDRSTRLEN);
        printf("Addr: %s\n", buf);
    }

    int s = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if(s < 0)
    {
        ouster_log("socket(): error\n");
        goto error;
    }

    int rc;
    if(desc->flags & NET_FLAGS_IPV6ONLY)
    {
        int off = 0;
        rc = setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));
        if(rc)
        {
            ouster_log("setsockopt(): error\n");
            goto error;
        }
    }

    if(desc->flags & NET_FLAGS_REUSE)
    {
        int option = 1;
        rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
        if(rc)
        {
            ouster_log("setsockopt(): error\n");
            goto error;
        }
    }

    if(desc->flags & NET_FLAGS_BIND)
    {
        rc = bind(s, info->ai_addr, (socklen_t)info->ai_addrlen);
        if(rc)
        {
            ouster_log("bind(): error\n");
            goto error;
        }
    }



    if(desc->flags & NET_FLAGS_NONBLOCK)
    {
        int flags = fcntl(s, F_GETFL, 0);
        if(flags == -1)
        {
            ouster_log("fcntl(): error\n");
            goto error;
        }
        rc = fcntl(s, F_SETFL, flags | O_NONBLOCK);
        if(rc == -1)
        {
            ouster_log("fcntl(): error\n");
            goto error;
        }
    }
    if(desc->rcvbuf_size)
    {
        rc = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&desc->rcvbuf_size, sizeof(desc->rcvbuf_size));
        if(rc)
        {
            ouster_log("setsockopt(): error\n");
            goto error;
        }
    }

    return s;

error:
    if(info)
    {
        ouster_log("freeaddrinfo()\n");
        freeaddrinfo(info);
    }
    if(s < 0)
    {
        ouster_log("close() socket\n");
        close(s);
    }
    return -1;
}





int64_t net_read(int sock, char * buf, int len)
{
    int64_t bytes_read = recv(sock, (char*)buf, len, 0);
    return bytes_read;
}



uint64_t net_select(int socks[], int n, const int timeout_sec)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    for(int i = 0; i < n; ++i)
    {
        FD_SET(socks[i], &rfds);
    }

    int max = 0;
    for(int i = 0; i < n; ++i)
    {
        if (socks[i] > max)
        {
            max = socks[i];
        }
    }

    uint64_t result = 0;
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    int rc = select((int)max + 1, &rfds, NULL, NULL, &tv);
    if(rc == -1)
    {
        ouster_log("select(): error\n");
        return 0;
    }
    for(int i = 0; i < n; ++i)
    {
        if (FD_ISSET(socks[i], &rfds))
        {
            result |= (1 << i);
        }
    }

    return result;
}
