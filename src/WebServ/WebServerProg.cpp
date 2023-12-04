
#include "WebServerProg.hpp"
#include <sys/socket.h>
#include <sys/poll.h>
#include <iostream>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "../Color.hpp"
#include "api_helpers.hpp"
#include "api_helpers.hpp"

# define MAXSOCKET 10

struct server;
void printServer(server &server);

static void errnoPrinting(std::string message, int error) 
{
	std::cerr << COLOR_RED << "Error! " << message << ": " << strerror(error) << COLOR_RESET << std::endl; //would errno segfault if unset?
}

void WebServerProg::addSocketToPoll(int socket, int event)
{
	struct pollfd fd;
	fd.fd = socket;
	fd.events = event;
	fd.revents = 0;
	m_pollSocketsVec.push_back(fd);
}

bool  WebServerProg::receiveRequest(int clientSocket) //server
{
	char buffer[1024];
	_request.clear(); // dummy data
	std::memset(buffer, 0, 1024);
	int bytes_received = recv(clientSocket, buffer, 1024, 0);
	std::cout << "in received Request" << bytes_received << " " << clientSocket << std::endl;
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;
		errnoPrinting("recv", errno);
		return false;
	}
	else if (bytes_received == 0)
	{
		std::cout << "Closing client socket" << std::endl;
		close(clientSocket);
		m_pollSocketsVec.pop_back();
	}
	else
	{
		std::string str = buffer;
		str += '\0';
		_request.append(buffer); // dummy data
		std::cout << str << std::endl;
	}
	return true;
}



// bool WebServ::receiveRequest(int clientSocket) //tuomo's addition, not connected
// {
// 	if (m_clientDataMap.find(clientSocket) == m_clientDataMap.end())
// 	{
// 		clientData data;
// 		m_clientDataMap.insert(std::make_pair(clientSocket, data));
// 	}
// 	char buffer[1024];

// 	_request.clear();
// 	memset(buffer, 0, 1024);
// 	int bytes_received = recv(clientSocket, buffer, 1024, 0);
// 	if (bytes_received < 0)
// 	{
// 		if (errno == EAGAIN || errno == EWOULDBLOCK)
// 		{
// 			return 1;
// 		}
// 		std::cout << "Error! recv" << std::endl;
// 		exit (1);
// 	}
// 	else if (bytes_received == 0)
// 	{
// 		std::cout << "Closing client socket" << std::endl;
// 		close(clientSocket);
// 		return 1;
// 	}
// 	else
// 	{
// 		std::string request = buffer;
// 		parseRequest(clientSocket, request);
// 	}
// 	return 0;
// }

void WebServerProg::sendResponse(int clientSocket)
{
	//const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Closed\r\nContent-Type: text/plain\r\n\r\nHello, World!";
	int	status = 200;
	int size = 0;

	std::string body;
	std::string path;

	// dummy data
	std::map<std::string, std::string> mp;
	mp["method"] = "GET";
	mp["path"] = extractPath(_request);

	
	// TODO --> add check if path and method is allowed. Can be done after merge of parsing
	// dummy data 
	path.append(mp["path"]);
	body = readFile(path, &status);
	std::cout << body;

	// use int status here and check config + read file before assigning status code
	_response.append("HTTP/1.1 ");
	_response.append(std::to_string(status));
	switch (status) {
	case 200:
		_response.append(" OK");
		break;
	case 404:
		_response.append(" Not Found");
	default:
		break;
	}
	_response.append("\r\n");

	// HANDLE ERROR HERE

	_response.append("Content-Length: ");
	size = body.size();
	_response.append(std::to_string(size));
	_response.append("\r\n");

	// This might be sent anyways
	_response.append("Connection: Closed");
	_response.append("\r\n");

	// will need to change this based on what we will return
	// TYPE WILL HAVE TO BE DETECTED 
	_response.append("Content-type: ");
	std::string type = getFileExtension(mp["path"]);
	std::cout << "type: " << type << "\n";
	if (type == ".html") {
		_response.append(TYPE_HTML);
	}
	else if (type == ".css") {
		_response.append(TYPE_CSS);
	}
	// 
	_response.append(END_HEADER);

	// use read content here
	_response.append(body);

	int bytes_sent = send(clientSocket, _response.c_str(), strlen(_response.c_str()), 0);
	if (bytes_sent < 0)
	{
		std::cout << "Error! send" << std::endl;
		exit(EXIT_FAILURE);
	}
	_response.clear();
}

void WebServerProg::initServers()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSocket < 0)
		{
			errnoPrinting("socket not created", errno);
			return ;
		}
		if (fcntl(listenSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
		{
			errnoPrinting("Fcntl", errno);
			return  ;
		}
		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(servers[i].port);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		int enable = 1;
		if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		{
			errnoPrinting("setsockopt(SO_REUSEADDR)", errno);
			return ;
		}
		if ((bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
		{
			errnoPrinting("Bind", errno);
			return ;
		}	 
		if (listen(listenSocket, MAXSOCKET) < 0)
		{
			errnoPrinting("Listen", errno);
			return  ;
		}
		servers[i].socketFD = listenSocket;
		addSocketToPoll(listenSocket, POLLIN);
		serverCount++;
	}
	return ;
}


int WebServerProg::acceptConnection(int listenSocket)
{
    int clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return -1;
        errnoPrinting("Accept", errno);
        return -1; 
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
    {
        errnoPrinting("fcntl", errno);
        //close(clientSocket);  
        return -1;
    }
    addSocketToPoll(clientSocket, POLLIN);
    std::cout << COLOR_GREEN << "New connection accepted on client socket " << clientSocket << COLOR_RESET << std::endl;
    return clientSocket;
}

void WebServerProg::runPoll()
{
	while (true)
	{
		int pollResult = poll(m_pollSocketsVec.data(), m_pollSocketsVec.size(), 100);
		if (pollResult < 0)
		{
			std::cout << "Error! poll" << std::endl;
			exit (1);
		}	 
		if (pollResult == 0)
			continue;
		for (size_t i  = 0; i < m_pollSocketsVec.size(); i++)
		{
			if (m_pollSocketsVec[i].revents & POLLIN)
			{
				if (i < serverCount)
				{
					addSocketToPoll(accept(m_pollSocketsVec[i].fd, NULL, NULL), POLLIN);
					int flags = fcntl(m_pollSocketsVec.back().fd, F_GETFL, 0);
					fcntl(m_pollSocketsVec.back().fd, F_SETFL, flags | O_NONBLOCK);
					std::cout << "New connection accepted on client socket" << std::endl;
				}
				else
				{
					std::cout << "Request: " << std::endl;
					receiveRequest(m_pollSocketsVec[i].fd);
					// if (receiveRequest(m_pollSocketsVec[i].fd))
					// 	continue;
					sendResponse(m_pollSocketsVec[i].fd);
					std::cout << "sent!!" << std::endl;
					// close(m_pollSocketsVec[i].fd);
					// m_clientDataMap.erase(m_pollSocketsVec[i].fd);
					// m_pollSocketsVec.erase(m_pollSocketsVec.begin() + i);
				}
			}
		}
	}

	// while (true)
	// {
	// 	int pollResult = poll(m_pollSocketsVec.data(), m_pollSocketsVec.size(), 100);
	// 	if (pollResult < 0)
	// 	{
	// 		errnoPrinting("Poll not created", errno);
	// 		return ;
	// 	}
	// 	if (pollResult == 0)
	// 		continue;
	// 	for (size_t i  = 0; i < m_pollSocketsVec.size() ; i++)
	// 	{
	// 		if (m_pollSocketsVec[i].revents & POLLIN) 
	// 		{
	// 			int fd = 0;
	// 			if (i < serverCount)
	// 				fd = acceptConnection(m_pollSocketsVec[i].fd); 
	// 			else
	// 				fd = m_pollSocketsVec[i].fd;
	// 			std::cout << "Request: " << std::endl;
	// 			std::cout << fd << std::endl;
	// 			receiveRequest(fd);
	// 			sendResponse(fd);
	// 			std::cout << COLOR_GREEN << "sent!!" << COLOR_RESET << std::endl;
	// 			close(fd);
	// 		}
	// 	}
	// }
}

void WebServerProg::startProgram()
{
	try
	{
		servers = parseConfigFile(defaultFileName);
		std::cout << COLOR_GREEN << "servers parsed" << COLOR_RESET << std::endl;
		validateServers(servers);
		std::cout << COLOR_GREEN << "servers valid" << COLOR_RESET << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
    {
        printServer(servers[i]);
    }
		initServers();
	}
	catch (const std::exception& e)
	{
		std::cout << COLOR_RED << "Error! " << e.what() << "Server cannot start" << COLOR_RESET << std::endl;
		return ;
	}	
	runPoll();
}


WebServerProg::WebServerProg() : serverCount(0)
{	
	defaultFileName = "DefaultConfig.conf";
}

WebServerProg::WebServerProg(std::string fileName)
{
	defaultFileName = fileName;
}

WebServerProg::~WebServerProg()
{
}
