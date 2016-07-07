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

int joinMulticast(struct sockaddr_in & serv, int sock)
{

//  //using IP_ADD_MEMBERSHIP
//  struct ip_mreq multicast;
//  bzero(&multicast, sizeof(multicast));
//  if (inet_pton(AF_INET, "172.173.1.1", &multicast.imr_interface) != 1)
//  {
//      cout << "inet_pton failed" << endl;
//      return 1;
//  }
//  if (inet_pton(AF_INET, "224.0.0.88", &multicast.imr_multiaddr) != 1)
//  {
//      cout << "inet_pton failed" << endl;
//      return 1;
//  }
//  ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast));
//  if (-1 == ret)
//  {
//      perror("setsockopt");
//      return errno;
//  }

    //using MCAST_JOIN_GROUP
    struct group_req multicast;
    bzero(&multicast, sizeof(multicast));
//    multicast.gr_interface = if_nametoindex("eth0");
    multicast.gr_interface = 0;
    memcpy(&multicast.gr_group, &serv, sizeof(serv));
    int ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, &multicast, sizeof(multicast));
    if (-1 == ret)
    {
        perror("setsockopt init to MCAST_JOIN_GROUP");
        return errno;
    }

    return 0;
}

#endif // MULTICAST_JOIN_H
