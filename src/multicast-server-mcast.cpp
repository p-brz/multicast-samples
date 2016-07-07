#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>

#include <iostream>

#include "multicast_common.h"
#include "multicast_join.h"

using namespace std;

int readMessages(int sock){
    struct sockaddr_in cli;
    bzero(&cli, sizeof(cli));
    socklen_t len = sizeof(cli);
    char buf[10240];

    printf("waiting connection\n");

    while (1) //TODO: remove infinite loop
    {
        bzero(buf, sizeof(buf));
        int n = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr *)&cli, &len);
        if (-1 == n)
        {
            perror("recvfrom");
            return errno;
        }

        printf("received %d bytes\n", n);

        char addr_buf[INET_ADDRSTRLEN];
        bzero(addr_buf, sizeof(addr_buf));
        if (inet_ntop(AF_INET, &cli.sin_addr, addr_buf, sizeof(addr_buf)))
        {
            printf("%s.%d: ", addr_buf, ntohs(cli.sin_port));
        }
        printf("%s\n", buf);

//      ret = setsockopt(sock, IPPROTO_IP, MCAST_LEAVE_GROUP, &multicast, sizeof(multicast));
//      if (-1 == ret)
//      {
//          perror("setsockopt MCAST_LEAVE_GROUP");
//          return errno;
//      }
    }

    //Not reachable
    return 0;
}

int main()
{
//    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (-1 == sock)
    {
        perror("socket");
        return errno;
    }

    sockaddr_storage mcast_addr;
    int joined = joinMulticast(sock, "224.0.0.88", "8888", SOCK_DGRAM, 0, &mcast_addr);
    int joined2 = joinMulticast(sock, "ff02::01", "8888", SOCK_DGRAM, 0, &mcast_addr);
    if(joined != 0 || joined2 != 0){
        return joined;
    }

//    int ret = bind(sock, (sockaddr *)&serv, sizeof(serv));

    addrinfo * addr = getAddress(NULL, "8888", SOCK_DGRAM, AF_INET6);

    char buffer[INET_ADDRSTRLEN];
    for(addrinfo * p = addr; p != NULL; p = p->ai_next){
        printf("Addr: %s\n", get_ip_str(p->ai_addr, buffer, INET6_ADDRSTRLEN));
    }

    int ret = bind(sock, addr->ai_addr, addr->ai_addrlen);
    if (-1 == ret)
    {
        perror("bind");
        return errno;
    }

    return readMessages(sock);
}
