

#include "WebServerProg.hpp"

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

void WebServerProg::addSocketToPoll(int socket, int event) //int listening
{
	struct pollfd fd;
	fd.fd = socket;
	fd.events = event;
	fd.revents = 0;
	m_pollSocketsVec.push_back(fd);

	//there needs to be a flag here that gets set to say if its a server or i client
}

void  WebServerProg::receiveRequest(int clientSocket)
{
	char buffer[1024];
	memset(buffer, 0, 1024);
	int bytes_received = recv(clientSocket, buffer, 1024, 0);
	std::cout << "in received Request" << bytes_received << " " << clientSocket << std::endl;
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		std::cerr << "Error! recv: " << strerror(errno) << std::endl;
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

void  WebServerProg::sendResponse(int clientSocket)
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

int WebServerProg::initServer() // this will need to be done using the input from config file this will take in the server map??
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
	server_addr.sin_port = htons(8888); // this will need to be done using the input from config file
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
	addSocketToPoll(listenSocket, POLLIN); //will only get here if successfully configured but is that correct
	return 0;
}

int WebServerProg::initServer2() // this will need to be done using the input from config file this will take in the server map??
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
	server_addr.sin_port = htons(8080); // this will need to be done using the input from config file
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
	addSocketToPoll(listenSocket, POLLIN); //will only get here if successfully configured but is that correct
	return 0;
}


int WebServerProg::acceptConnection(int listenSocket)
{
    int clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return -1; // No new connection, continue polling
        std::cerr << "Error! accept: " << strerror(errno) << std::endl;
        return -1; //dont exit
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
    {
        std::cerr << COLOR_RED << " fcntl" << COLOR_RESET << std::endl;
        close(clientSocket);
        return -1;
    }
    addSocketToPoll(clientSocket, POLLIN);
    std::cout << COLOR_GREEN << "New connection accepted on client socket " << clientSocket << COLOR_RESET << std::endl;
    return clientSocket;
}

void WebServerProg::startProgram()
{
	//parse the file into the vector of maps from the 
	//init the individual servers
	int returnValue = initServer();
	if (returnValue < 0)
		std::cerr << COLOR_RED << "Server can no start" << COLOR_RESET << std::endl;
	std::cout << COLOR_GREEN << "New connection on client socket serverinit	"<< returnValue << COLOR_RESET << std::endl;
	returnValue = initServer2();
	if (returnValue < 0)
		std::cerr << COLOR_RED << "Server can no start" << COLOR_RESET << std::endl;
	std::cout << COLOR_GREEN << "New connection on client socket serverinit2	" << returnValue << COLOR_RESET << std::endl;
	
	while (true) //needs to check all in poll for events
	{
		int pollResult = poll(m_pollSocketsVec.data(), m_pollSocketsVec.size(), 100); //how does this work
		if (pollResult < 0)
		{
			std::cerr << COLOR_RED << "Error! Poll not created" << COLOR_RESET << std::endl;
			exit (1);
		}
		if (pollResult == 0)
			continue;
		for (size_t i  = 0; i < m_pollSocketsVec.size() ; i++)
		{

			std::cout << "m_pollSocketsVec[i].events: " << m_pollSocketsVec[i].events << std::endl;
			
			if (m_pollSocketsVec[i].revents & POLLIN) //WHY DOES REQUEST BREAK, okay it need the accept
			{
				if (m_pollSocketsVec[i] == LISTENFLAG)
					acceptConnection(m_pollSocketsVec[i]); //BUT HOW DO I FLAG IT
				std::cout << "Request: " << std::endl;
				std::cout << m_pollSocketsVec[i].fd << std::endl;
				receiveRequest(m_pollSocketsVec[i].fd);
				sendResponse(m_pollSocketsVec[i].fd);
				std::cout << COLOR_GREEN << "sent!!" << COLOR_RESET << std::endl;
				close(m_pollSocketsVec[i].fd);
			}
		}
	}
}


WebServerProg::WebServerProg()
{
	//this will have the default config file
}

// WebServerProg::WebServerProg(std::string filename)
// {
// 	//open the file etc (it will be pre checked)
// }

WebServerProg::~WebServerProg()
{
}
