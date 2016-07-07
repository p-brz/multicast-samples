/* Send Multicast Datagram code example. */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "multicast_common.h"

using namespace std;

int createSocket(int family = AF_INET6){
//    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    int sd = socket(family, SOCK_DGRAM, 0);

    if (sd < 0){
        perror("Opening datagram socket error");
        exit(1);
    }
    else{
        printf("Opening the datagram socket...OK.\n");
    }

    return sd;
}

uint16_t getDestAddr_ipv4(struct sockaddr_in & groupSock, const char* remotePort, const char* remoteAddress)
{
    socklen_t sockSize = sizeof(groupSock);
    memset((char *)&groupSock, 0, sockSize);
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr(remoteAddress);

    uint16_t portNum = (uint16_t)atoi(remotePort);
    groupSock.sin_port = htons(portNum);

    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */

    struct in_addr localInterface;
    localInterface.s_addr = INADDR_ANY;

    return portNum;
}

sockaddr_storage * getDestAddr(struct sockaddr_storage * groupSock, const char* remotePort, const char* remoteAddress)
{
    memset(groupSock, 0, sizeof(sockaddr_storage));

    addrinfo * info = getAddress(remoteAddress, remotePort, SOCK_DGRAM);

    if(info != NULL){
        memcpy(groupSock, info->ai_addr, info->ai_addrlen);
        return groupSock;
    }

    return NULL;
}

int main(int argc, char *argv[]){
    /* Create a datagram socket on which to send. */
    int sd = createSocket();

    const char * remoteAddress = argc > 2 ? argv[2] : "226.1.1.1";
    const char * remotePort = argc > 3 ? argv[3] : "4321";

    /* Initialize the group sockaddr structure with a */
    /* group address of 225.1.1.1 and port 5555. */
    struct sockaddr_storage groupSock;
    if(getDestAddr(&groupSock, remotePort, remoteAddress)==NULL){
        perror("Failed to get addr");
        return -1;
    }

    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */
    /**
    struct in_addr localInterface;
    localInterface.s_addr = INADDR_ANY;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
    {
        perror("Setting local interface error");
        exit(1);
    }
    else{
        printf("Setting the local interface...OK\n");
    }
    */

    /* Send a message to the multicast group specified by the*/
    /* groupSock sockaddr structure. */
    //char databuf[1024] = "Multicast test message lol!";
    //int datalen = sizeof(databuf);

    string message;
    if(argc > 1){
        message = argv[1];
    }
    else{
//        cout << "Data to send: " << endl;
//        std::getline( std::cin, message);
        message = "Hello world!";
    }

    cout << "Sending: \"" << message << "\" to " << remoteAddress << " at " << remotePort << endl;

    if (sendto(sd, message.c_str(), message.length(), 0, (struct sockaddr *)&groupSock, sizeof(groupSock)) < 0)
    {
        perror("Sending datagram message error");
    }
    else{
        printf("Sending datagram message...OK\n");
    }

    return 0;
}

