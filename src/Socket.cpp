
#include "Socket.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

fd_set fileRead, fWrite, filesExcpetion; //This will probably be a server base class

static void *getSenderAddress(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        // If IPv4, return a pointer to the IPv4 address in the sockaddr_in structure
        return &(((struct sockaddr_in *)sa)->sin_addr);
    // If IPv6, return a pointer to the IPv6 address in the sockaddr_in6 structure
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
//fd_set

void Socket::listenForSender()
{
    //struct sockaddr_in senderAddress; // Storage for sender's address
    
	// senderAddress.sin_family = AF_INET;
	// senderAddress.sin_port = htons(MYPORT);
	// senderAddress.sin_addr.s_addr = INADDR_ANY;
	// memset(&(senderAddress.sin_zero), 0, 8);    //MIGHT NOT NEED ANY OF THIS
	int returnValue = 1;
	while (returnValue)
	{	
		// addressLen = sizeof senderAddress;
		returnValue = listen(socketFd, BACKLOG);
		if (returnValue == -1) 
		{
			std::cerr << "recvfrom" << std::endl;
			exit(1);
		}
		else
		{
			std::cout << "Listening to local port " <<	returnValue  << std::endl;
		}
		

	}

}


int Socket::createBindSocket( struct addrinfo defaults)
{
    struct addrinfo *serverInfo, *p; 
    
    // Get address information for the server
    // The getaddrinfo function, when successful, returns a linked list of address structures (struct addrinfo)
    int resultsValue = 0;
    if ((resultsValue = getaddrinfo(NULL, "8080", &defaults, &serverInfo)) != 0) //myport string? but could potentially change
    {
        std::cerr << "getaddrinfo: " << gai_strerror(resultsValue) << std::endl;
        return 1;
    }
    // Loop through all the results and bind to the first available address
    // Each node in this linked list contains information about a possible socket address that can be used for binding.
    for (p = serverInfo; p != NULL; p = p->ai_next) //this wont stay because we will have config file stuff?
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
