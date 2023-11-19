// #include "SocketHandler.hpp"
// #include <iostream>

// ///demo stuff, check whats needed
// #include <sys/types.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <fcntl.h>
// #include <unistd.h>

// #include <sys/socket.h>
// #include <netinet/in.h> //needed for sockaddr_in struct etc
// #define MAXBUFLEN 100
// // struct sockaddr_in 
// // { 
// // 	__uint8_t         sin_len; 
// // 	sa_family_t       sin_family; //The address family we used when we set up the socket. In demo case, it’s AF_INET.
// // 	in_port_t         sin_port; /* port number: You can explicitly assign a transport address (port) or allow the operating system to assign one.
// // 								If you’re a client and won’t be receiving incoming connections, 
// // 								you’ll usually just let the operating system pick any available port number by specifying port 0.
// // 								If you’re a server, you’ll generally pick a specific number since clients will need to know a 
// // 								port number to connect to. */
// // 	struct in_addr    sin_addr;  //address for this socket:  machine’s IP address. 
// // 	char              sin_zero[8]; 
// // };


// #define MYPORT 4950

// long		socketAccept(int fd)
// {
// 	long	socket = 0;
// 	std::cout << "in accept" << std::endl;
// 	socket = accept(fd, NULL, NULL);
// 	std::cout << "socket = "<< socket << std::endl;
// 	if (socket == -1)
// 		std::cerr <<  "Could not create socket. " << std::endl;
// 	else
// 	{
// 		std::cout << "in accept" << std::endl;
// 		fcntl(socket, F_SETFL, O_NONBLOCK); //what?
// 	}
// 	return socket;
// }

// int connectToNetwork(int serverSocket, struct sockaddr_in address)
// {
// 	if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) 
// 	{ 
// 		std::cerr << "creating socket failed" << std::endl;
// 		serverSocket = FAILED;
// 		return 1;
// 	}
// 	return 0;
// }

// // void *get_in_addr(struct sockaddr *sa)
// // {
// //     if (sa->sa_family == AF_INET) {
// //         return &(((struct sockaddr_in*)sa)->sin_addr);
// //     }

// //     return &(((struct sockaddr_in6*)sa)->sin6_addr);
// // }

// void	SocketHandler::createBindSocket(int addrFamily, int virtCircSock, int protocol) //should this also take the port? hows that working
// {

// 	struct sockaddr_in address, their_addr; 
	
// 	int serverSocket = socket(addrFamily, virtCircSock, 0);
// 	address.sin_family = addrFamily;
// 	address.sin_addr.s_addr = htonl(INADDR_ANY);
// 	address.sin_port = htons(MYPORT);


// 	//int serverSocket = socket(addrFamily, virtCircSock, protocol);
// 	if (serverSocket < 0) //validates the established connection
// 	{
// 		std::cerr << "creating socket failed" << std::endl;
// 		serverSocket = FAILED;
// 	}
// 	if (connectToNetwork(serverSocket, address) != FAILED)
// 	{
// 		serverFd.push_back(serverSocket); //probably not great
// 		listen(serverSocket, BACKLOG);
// 		char buf[MAXBUFLEN];

// 		char s[INET6_ADDRSTRLEN];

// 		while (1) //server is running
// 		{
// 			std::cout << "listener: waiting to recvfrom." << std::endl;
// 			int clientSocket = socketAccept(serverSocket);
			
// 			if (clientSocket == -1)
// 			{
// 				std::cerr << "creating socket failed" << std::endl;
// 			}

// 			socklen_t addr_len = sizeof their_addr;
// 			int numbytes = 0;
// 		if ((numbytes = recv(clientSocket, buf, MAXBUFLEN-1 , 0)) == -1)
// 		{
// 			perror("recvfrom"); //nope
// 			exit(1);
// 		}

// 			printf("listener: got packet from %s\n", inet_ntop(their_addr.sin_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
// 			printf("listener: packet is %d bytes long\n", numbytes);
// 			buf[numbytes] = '\0';
// 			printf("listener: packet contains \"%s\"\n", buf);
// 			close(serverSocket);
// 		}
// 	}
// }



// SocketHandler::SocketHandler()
// {
// 	/* 
// 	if domain, socket type etc change we could have an Vector of socket classes inside main program, then call contruct 
// 	constructor would need to init the create/bind variables including address
// 	*/
// }

// // SocketHandler::SocketHandler( SocketHandler const & src )
// // {
// // 	this = src; 
// // }

// SocketHandler::~SocketHandler()
// {}


// /* stash ip addresses in a vector of double that take a parse ip address with points removed......
// if more thann 5 connections from single IP addresses then we would reject the connection based on you used too many active connections*/