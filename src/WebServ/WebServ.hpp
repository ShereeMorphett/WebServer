#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <poll.h>
#include <vector>
#include <map>
#include <string>

struct clientData
{
	std::multimap<std::string, std::string> requestData;
};

class WebServ
{
private:
	std::vector<struct pollfd> m_pollSocketsVec;
	std::map<int, struct clientData> m_clientDataMap;
	std::string	_response;
	std::string	_request;

	
	void initListenSocket();
	void addSocketToPoll(int socket, int event);
	void parseRequest(int clientSocket, std::string request);
	bool receiveRequest(int clientSocket);
	void sendResponse(int clientSocket);
public:
	WebServ() {}
	void start();
};


#endif