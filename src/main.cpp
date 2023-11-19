
// 	this is a placeholder main for the socket handler class

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "Socket.hpp"


// Function to retrieve the sender's address from a sockaddr structure
void *getSenderAddress(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        // If IPv4, return a pointer to the IPv4 address in the sockaddr_in structure
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    // If IPv6, return a pointer to the IPv6 address in the sockaddr_in6 structure
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{

    struct addrinfo defaults;
    memset(&defaults, 0, sizeof defaults);
    defaults.ai_family = AF_INET6; // Use IPv6
    defaults.ai_socktype = SOCK_DGRAM; // Use UDP
    defaults.ai_flags = AI_PASSIVE; // Use the local host's address

    Socket serverSocket(defaults);
    serverSocket.listenForSender();
    // serverSocket.handleConnections();

    return 0;
}