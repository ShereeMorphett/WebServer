
// /* 
// 	this is a placeholder main for the socket handler class
// */
// #include "SocketHandler.hpp"
// #include <sys/socket.h>

// int main ()
// {

// 	SocketHandler sockets;
	
// 	sockets.createBindSocket(AF_INET, SOCK_STREAM, 0);

// 	return 0;
// }

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "4950"
#define MAXBUFLEN 100


//this whole thing needs rewrting

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0)
	{
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
	{
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "socket: " << sockfd << std::endl;
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
            close(sockfd);
			std::cerr << "listener: " << sockfd << std::endl;
            continue;
        }
        break;
    }

    if (p == NULL) {
        std::cerr << "listener: failed to bind socket" << std::endl;
        return 2;
    }

    freeaddrinfo(servinfo);

    std::cout << "listener: waiting to recvfrom..." << std::endl;

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    std::cout << "listener: got packet from " << inet_ntop(their_addr.ss_family,
                                                            get_in_addr((struct sockaddr *)&their_addr),
                                                            s, sizeof s) << std::endl;
    std::cout << "listener: packet is " << numbytes << " bytes long" << std::endl;
    buf[numbytes] = '\0';
    std::cout << "listener: packet contains \"" << buf << "\"" << std::endl;

    close(sockfd);

    return 0;
}
