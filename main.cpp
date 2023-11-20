
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






int main()
{

    struct addrinfo defaults;
    memset(&defaults, 0, sizeof defaults);
    defaults.ai_family = AF_INET6; // Use IPv6, which should it be
    defaults.ai_socktype = SOCK_STREAM; //Useds TCP/IP  
    defaults.ai_flags = AI_PASSIVE; // Use the local host's address, this would probably need to change with config file??

    Socket serverSocket(defaults);
    serverSocket.listenForSender();
    // serverSocket.handleConnections();

    return 0;
}