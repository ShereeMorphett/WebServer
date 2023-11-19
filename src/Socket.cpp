
#include "Socket.hpp"

#include <iostream>
// #include <cstring>
// #include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static void *getSenderAddress(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        // If IPv4, return a pointer to the IPv4 address in the sockaddr_in structure
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    // If IPv6, return a pointer to the IPv6 address in the sockaddr_in6 structure
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


void Socket::listenForSender()
{
    struct sockaddr_storage senderAddress;// Storage for sender's address
    int numbytes; // Number of received bytes 
    char buf[MAXBUFLEN]; // Buffer to store received data
    char s[INET6_ADDRSTRLEN]; // String to store the sender's IP address
    socklen_t addressLen;

    addressLen = sizeof senderAddress;  // Length of the sender's address
    if ((numbytes = recvfrom(socketFd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&senderAddress, &addressLen)) == -1)
    {
        std::cerr << "recvfrom" << std::endl;
        exit(1);
    }

    // Display information about the received packet
    std::cout << "listener: got packet from " << inet_ntop(senderAddress.ss_family, getSenderAddress((struct sockaddr *)&senderAddress), s, sizeof s) << std::endl;
    std::cout << "listener: packet is " << numbytes << " bytes long" << std::endl;
    buf[numbytes] = '\0';
    std::cout << "listener: packet contains \"" << buf << "\"" << std::endl;


}


int Socket::createBindSocket( struct addrinfo defaults)
{
    struct addrinfo *serverInfo, *p; 
    
    // Get address information for the server
    // The getaddrinfo function, when successful, returns a linked list of address structures (struct addrinfo)
    int resultsValue = 0;
    if ((resultsValue = getaddrinfo(NULL, MYPORT, &defaults, &serverInfo)) != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(resultsValue) << std::endl;
        return 1;
    }
    // Loop through all the results and bind to the first available address
    // Each node in this linked list contains information about a possible socket address that can be used for binding.
    for (p = serverInfo; p != NULL; p = p->ai_next)
    {
        if ((socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            std::cerr << "socket: " << socketFd << std::endl;
            continue;
        }
        if (bind(socketFd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(socketFd);
            std::cerr << "listener: " << socketFd << std::endl;
            continue;
        }
        // If successful, break out of the loop
        break;
    }
    if (p == NULL)
    {
        std::cerr << "Failed to bind a socket" << std::endl;
        return 2;
    }
    freeaddrinfo(serverInfo); //dunno if neccessary in cpp
    return 0;
}

Socket::Socket(struct addrinfo defaults)
{
   createBindSocket(defaults);
    //this will need error handling
    
}

Socket::~Socket()
{
    close(socketFd);
}
