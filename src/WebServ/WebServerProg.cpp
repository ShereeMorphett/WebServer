
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

void  WebServerProg::receiveRequest(int clientSocket) //server
{
	char buffer[1024];
	std::memset(buffer, 0, 1024);
	int bytes_received = recv(clientSocket, buffer, 1024, 0);
	std::cout << "in received Request" << bytes_received << " " << clientSocket << std::endl;
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		errnoPrinting("recv", errno);
		return ;
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
		std::cout << str << std::endl;
	}
}

void  WebServerProg::sendResponse(int clientSocket)
{
	const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Closed\r\nContent-Type: text/plain\r\n\r\nHello, World!";
	
	std::cout << COLOR_CYAN << "sending response..." << COLOR_RESET << std:: endl;
	int bytes_sent = send(clientSocket, response, strlen(response), 0);
	if (bytes_sent < 0)
	{
		errnoPrinting("Response did not send", errno);
		return ;
	}
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
    int clientSocket = accept(listenSocket, NULL, NULL); //, POLLIN); //okay but does it HAVE to have POLLIN and why doesnt it work
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
        close(clientSocket);
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
			errnoPrinting("Poll not created", errno);
			return ;
		}
		if (pollResult == 0)
			continue;
		for (size_t i  = 0; i < m_pollSocketsVec.size() ; i++)
		{
			if (m_pollSocketsVec[i].revents & POLLIN) 
			{
				int fd = 0;
				if (i < serverCount)
					fd = acceptConnection(m_pollSocketsVec[i].fd); 
				else
					fd = m_pollSocketsVec[i].fd;
				std::cout << "Request: " << std::endl;
				std::cout << fd << std::endl;
				receiveRequest(fd);
				sendResponse(fd);
				std::cout << COLOR_GREEN << "sent!!" << COLOR_RESET << std::endl;
				close(fd);
			}
		}
	}
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
