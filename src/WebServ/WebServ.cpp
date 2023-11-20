#include "WebServ.hpp"
#include <sys/socket.h>
#include <sys/poll.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <cerrno>
#include "../Color.hpp"

#define MAXSOCK 10
#define PORT 8888

void WebServ::addSocketToPoll(int socket, int event)
{
	struct pollfd fd;
	fd.fd = socket;
	fd.events = event;
	fd.revents = 0;
	m_pollSocketsVec.push_back(fd);
}

void WebServ::receiveRequest(int clientSocket)
{
	char buffer[1024];
	memset(buffer, 0, 1024);

	int bytes_received = recv(clientSocket, buffer, 1024, 0);
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		std::cout << "Error! recv" << std::endl;
		exit (1);
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

void WebServ::sendResponse(int clientSocket)
{
	const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Closed\r\nContent-Type: text/plain\r\n\r\nHello, World!";
	
	std::cout << COLOR_CYAN << "sending response..." << COLOR_RESET << std:: endl;
	int bytes_sent = send(clientSocket, response, strlen(response), 0);
	if (bytes_sent < 0)
	{
		std::cerr << COLOR_RED << "Error! Response did not send" << COLOR_RESET << std::endl;
		exit(EXIT_FAILURE);
	}
}

int WebServ::initListenSocket()
{
	int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket < 0)
	{
		std::cerr << COLOR_RED << "Error! socket not created" << COLOR_RESET << std::endl;
		return -1;
	}
    if (fcntl(listenSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
	{
		std::cerr << COLOR_RED << " fcntl" << COLOR_RESET << std::endl;
		return -1;
    }

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int enable = 1;

	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::cerr << COLOR_RED << "Error! setsockopt(SO_REUSEADDR)" << COLOR_RESET << std::endl;
		return -1;
	}
	if ((bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
	{
		std::cerr << COLOR_RED << "Error! bind" << COLOR_RESET << std::endl;
		return -1;
	}	 
	if (listen(listenSocket, 5) < 0) //why is it 5? pretty sure it should be something bigger? MAXSOCK
	{
		std::cerr << COLOR_RED << "Error! Listen" << COLOR_RESET << std::endl;
		return -1;
	}
	addSocketToPoll(listenSocket, POLLIN);
	return 0;
}


void WebServ::start()
{
	int returnValue = initListenSocket();
	if (returnValue < 0)
		std::cerr << COLOR_RED << "Server can no start" << COLOR_RESET << std::endl;
	while (true)
	{
		int pollResult = poll(m_pollSocketsVec.data(), m_pollSocketsVec.size(), 100);
		if (pollResult < 0)
		{
			std::cerr << COLOR_RED << "Error! Poll not created" << COLOR_RESET << std::endl;
			exit (1);
		}	 
		if (pollResult == 0)
			continue;
		for (size_t i  = 0; i < m_pollSocketsVec.size() && m_pollSocketsVec.size() < MAXSOCK; i++)
		{
			if (m_pollSocketsVec[i].revents & POLLIN)
			{
				if (i == 0)
				{
					addSocketToPoll(accept(m_pollSocketsVec[i].fd, NULL, NULL), POLLIN);
					if (fcntl(m_pollSocketsVec.back().fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
					{
						std::cerr << COLOR_RED << " fcntl" << COLOR_RESET << std::endl;
						return ;
					}
					std::cout << COLOR_GREEN << "New connection accepted on client socket" << COLOR_RESET << std::endl;
				}
				else
				{
					std::cout << "Request: " << std::endl;
					receiveRequest(m_pollSocketsVec[i].fd);
					sendResponse(m_pollSocketsVec[i].fd);
					std::cout << COLOR_GREEN << "sent!!" << COLOR_RESET << std::endl;
					close(m_pollSocketsVec[i].fd);
					//m_pollSocketsVec.erase(i); //should free the spot in the socket array?

				}
			}
		}
	}
	
}
