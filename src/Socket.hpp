#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <vector>
#define BACKLOG 10
#define MAXBUFLEN 100
#define MYPORT 8080

class Socket
{
    private:
        int socketFd;
		fd_set fileRead, fWrite, filesExcpetion; //This will probably be a server base class
        std::vector<int> connectedSockets; //dunno if that needs to be inside the class or in the main?
    public:
        int createBindSocket(struct addrinfo defaults);
        void listenForSender();
        int getSocketFd() {return socketFd;};
        Socket(struct addrinfo defaults);
        ~Socket();
};

#endif
