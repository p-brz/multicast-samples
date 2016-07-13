#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <stdlib.h>

#include <iostream>
#include <string>

#include "multicast_common.h"
#include "multicast_join.h"

#define IPV4_MULTI_ADDR "224.0.0.88"
#define IPV6_MULTI_ADDR "ff02::01"

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

void bindSocket(int sock, int port, const std::string & address=""){
    const std::string portStr = std::to_string(port);
    const char  * addrStr = !address.empty() ? address.c_str() : NULL;

    addrinfo * addr = getAddress(NULL, portStr.c_str(), SOCK_DGRAM, AF_INET6);

    int ret = bind(sock, addr->ai_addr, addr->ai_addrlen);
    if (-1 == ret){
        perror("bind");
        exit(errno);
    }
}

void checkError(int retValue, const char* message){
    if (retValue == -1){
        perror(message);
        exit(errno);
    }
}

int buildSocket(int family = AF_INET6){
    int sock = socket(family, SOCK_DGRAM, 0);
    checkError(sock, "socket");

    int reuse = true;
    checkError(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)), "reuse addr");


    return sock;
}

int main()
{
    int sock = buildSocket();

    printf("build socket\n");

    sockaddr_storage mcast_addr;
    int joined = joinMulticast(sock, IPV4_MULTI_ADDR, "8888", SOCK_DGRAM, 0, &mcast_addr);
    int joined2 = joinMulticast(sock, IPV6_MULTI_ADDR, "8888", SOCK_DGRAM, 0, &mcast_addr);
    if(joined != 0 || joined2 != 0){
        return joined;
    }

    printf("joined multicast addresses\n");

    bindSocket(sock, 8888);

    printf("bound socket\n");

    return readMessages(sock);
}
