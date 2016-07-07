#ifndef MULTICAST_COMMON_H
#define MULTICAST_COMMON_H

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>

inline char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, maxlen);
            break;

        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }

    return s;
}

inline struct addrinfo * getAddress(const char * address, const char * port, int sockType, int sockFamily = AF_UNSPEC){
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = sockFamily;
    hints.ai_socktype = sockType;

    if(address == NULL){
        hints.ai_flags = AI_PASSIVE;
    }

    int rv =0;
    struct addrinfo *addr = NULL;
    if ((rv = getaddrinfo(address, port, &hints, &addr)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    return addr;
}

#endif // MULTICAST_COMMON_H
