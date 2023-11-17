#include "SocketHandler.hpp"
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h> //needed for sockaddr_in struct etc

// struct sockaddr_in 
// { 
// 	__uint8_t         sin_len; 
// 	sa_family_t       sin_family; //The address family we used when we set up the socket. In demo case, it’s AF_INET.
// 	in_port_t         sin_port; /* port number: You can explicitly assign a transport address (port) or allow the operating system to assign one.
// 								If you’re a client and won’t be receiving incoming connections, 
// 								you’ll usually just let the operating system pick any available port number by specifying port 0.
// 								If you’re a server, you’ll generally pick a specific number since clients will need to know a 
// 								port number to connect to. */
// 	struct in_addr    sin_addr;  //address for this socket:  machine’s IP address. 
// 	char              sin_zero[8]; 
// };


void	SocketHandler::socketListen(int fd)
{
	listen(fd, BACKLOG);

}

void	SocketHandler::createBindSocket(int addrFamily, int virtCircSock, int protocol) //should this also take the port? hows that working
{
	int serverSocket = socket(addrFamily, virtCircSock, protocol);

	if (serverSocket < 0) 
	{
		std::cerr << "creating socket failed" << std::endl;
		serverSocket = FAILED;
	}

	struct sockaddr_in address; 
	const int PORT = 8080; //Where the clients can reach  does this ever change?

	//defining address structure
	address.sin_family = addrFamily;
	address.sin_addr.s_addr = htonl(INADDR_ANY);  //in address_any? meaning needs to be unsigned long that is interface/my comps IP
	address.sin_port = htons(PORT); 

	if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) 
	{ 
		std::cerr << "creating socket failed" << std::endl;
		serverSocket = FAILED; 
	}
	else
		serverFd.push_back(serverSocket);
}

int SocketHandler::testConnect(int testItem) //HUH?
{
	if (testItem < 0) 
		std::cerr << "Connection failed" << std::endl;
}


SocketHandler::SocketHandler()
{
	/* 
	if domain, socket type etc change we could have an Vector of socket classes inside main program, then call contruct 
	constructor would need to init the create/bind variables including address
	*/
}

// SocketHandler::SocketHandler( SocketHandler const & src )
// {
// 	this = src; 
// }

SocketHandler::~SocketHandler()
{}


/* stash ip addresses in a vector of double that take a parse ip address with points removed......
if more thann 5 connections from single IP addresses then we would reject the connection based on you used too many active connections*/