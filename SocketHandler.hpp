

#ifndef SocketHandler_HPP
# define SocketHandler_HPP
# include <vector>
# include <iostream>

#define FAILED -1
#define BACKLOG 100 // NGINX IS 511 BUT TESTING 
/*defines the maximum number of pending connections that can be queued up before connections are refused.*/

class SocketHandler
{
	private:
		std::vector<int> serverFd; //size of ? for manag
		std::vector<double> usedIPaddress; //better name please
		int connection;

	public:
		void	createBindSocket(int addrFamily, int virtCircSock, int protocol);
		void	socketListen(int fd);
		SocketHandler();
//		SocketHandler( SocketHandler const & src );
		~SocketHandler();
		void networkConnection();
		int testConnect(int testItem);
//		SocketHandler &	operator=( SocketHandler const & right); 

};



#endif