#ifndef MULTICAST_JOIN_H
#define MULTICAST_JOIN_H

#include "multicast_common.h"

int joinMulticast(int sock, struct sockaddr * addr, socklen_t sockSize, int interfaceIndex=0){
    struct group_req multicast;
    bzero(&multicast, sizeof(multicast));
    multicast.gr_interface = interfaceIndex;

    memcpy(&multicast.gr_group, addr, sockSize);

//    int ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, &multicast, sizeof(multicast));
    int level = (addr->sa_family == AF_INET) ? IPPROTO_IP : IPPROTO_IPV6;
    int ret = setsockopt(sock, level, MCAST_JOIN_GROUP, &multicast, sizeof(multicast));
    if (-1 == ret)
    {
        perror("setsockopt init to MCAST_JOIN_GROUP");
        return errno;
    }

    return 0;
}

int joinMulticast(int sock, const char * address, const char * port, int sockType, int interfaceIndex = 0, sockaddr_storage * resultAddr = NULL){
    struct addrinfo * addr = getAddress(address, port, sockType);

    int joined = -1;
    struct addrinfo *p = addr;
    while(p != NULL && joined < 0){
        joined = joinMulticast(sock, p->ai_addr, p->ai_addrlen, interfaceIndex);
        if(joined == 0){
            printf("Joined group(%s)!\n", address);

            if(resultAddr != NULL){
                memset(resultAddr, 0, sizeof(sockaddr_storage));
                memcpy(resultAddr, p->ai_addr, p->ai_addrlen);
            }
        }
        else{
            char addrStr[INET6_ADDRSTRLEN];
            get_ip_str(p->ai_addr, addrStr, INET6_ADDRSTRLEN);
            printf("Failed to join multicast group(%s)\n", addrStr);
        }

        p = p->ai_next;
    }

    freeaddrinfo(addr); // all done with this structure

    return joined;
}

#endif // MULTICAST_JOIN_H
