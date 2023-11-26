#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <poll.h>
#include <vector>
#include <string>

class WebServ
{
private:
	std::vector<struct pollfd> m_pollSocketsVec;
	std::string	_response;
	std::string	_request;


	
	void initListenSocket();
	void addSocketToPoll(int socket, int event);
	void receiveRequest(int clientSocket);
	void sendResponse(int clientSocket);
public:
	WebServ() {}
	void start();
};


#endif