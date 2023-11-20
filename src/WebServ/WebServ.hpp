#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <poll.h>
#include <vector>

class WebServ
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;//moved
		
		int initListenSocket();
		void addSocketToPoll(int socket, int event); //moved
		void receiveRequest(int clientSocket);
		void sendResponse(int clientSocket);

	public:
		WebServ() {}
		void start();
};


#endif