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
#include <map>

#include "api_helpers.hpp"
#include "constants.hpp"

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

	_request.clear();
	memset(buffer, 0, 1024);
	int bytes_received = recv(clientSocket, buffer, 1024, 0);
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return ;
		}
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
		_request = str;
		std::cout << str << std::endl;
	}
}

void WebServ::sendResponse(int clientSocket)
{
	// const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Closed\r\nContent-Type: text/plain\r\n\r\nHello, World!";
	int	status = 200;
	int size = 0;

	std::string body;
	std::string path;


	std::map<std::string, std::string> mp;
	mp["method"] = "GET";
	mp["path"] = extractPath(_request);

	
	// TODO --> add check if path and method is allowed. Can be done after merge of parsing

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

void WebServ::initListenSocket()
{
	int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket < 0)
	{
		std::cout << "Error! socket" << std::endl;
		exit (1);
	}

	int flags = fcntl(listenSocket, F_GETFL, 0);
    if (flags == -1)
	{
        perror("fcntl F_GETFL");
		exit (-1);
    }

    if (fcntl(listenSocket, F_SETFL, flags | O_NONBLOCK) == -1)
	{
        perror("fcntl F_SETFL O_NONBLOCK");
		exit (-1);
    }

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int enable = 1;
	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::cout << "Error! setsockopt(SO_REUSEADDR)" << std::endl;
		exit (1);
	}
	if ((bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
	{
		std::cout << "Error! bind" << std::endl;
		exit (1);
	}	 

	if (listen(listenSocket, 5) < 0)
	{
		std::cout << "Error! listen" << std::endl;
		exit (1);
	}
	addSocketToPoll(listenSocket, POLLIN);
}


void WebServ::start()
{
	initListenSocket();
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
				if (i == 0)
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
					sendResponse(m_pollSocketsVec[i].fd);
					std::cout << "sent!!" << std::endl;
					close(m_pollSocketsVec[i].fd);
				}
			}
		}
	}
}
