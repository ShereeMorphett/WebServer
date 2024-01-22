
#include "WebServerProg.hpp"
#include <sys/socket.h>
#include <sys/poll.h>
#include <iostream>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>//
#include <cstdlib> //
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include <fcntl.h>
#include <vector>
#include "../Color.hpp"
#include "api_helpers.hpp"
#include "utils.hpp"
#include "CgiHandler.hpp"
#include <sys/stat.h>

# define MAXSOCKET 25


static void errnoPrinting(std::string message, int error) 
{
	std::cerr << COLOR_RED << "Error! " << message << ": " << strerror(error) << COLOR_RESET << std::endl;
}

void WebServerProg::addSocketToPoll(int socket, int event)
{
	struct pollfd fd;
	fd.fd = socket;
	fd.events = event;
	fd.revents = 0;
	m_pollSocketsVec.push_back(fd);
}

void WebServerProg::initClientData(int clientSocket, int serverIndex)
{
	clientData data;
	data.serverIndex = serverIndex;
	m_clientDataMap.insert(std::make_pair(clientSocket, data));
}

std::string WebServerProg::accessDataInMap(int clientSocket, std::string header)
{
    std::map<int, clientData>::iterator clientIt = m_clientDataMap.find(clientSocket);

    if (clientIt != m_clientDataMap.end())
    {
        std::map<std::string, std::string>::iterator headerIt = clientIt->second.requestData.find(header);
        if (headerIt != clientIt->second.requestData.end())
        {
            return headerIt->second;
        }
    }
    std::cout << COLOR_RED << "Not found- error issues" << COLOR_RESET << std::endl;
	std::cout << COLOR_YELLOW << header << COLOR_RESET << std::endl;
    return NULL;
}


void	WebServerProg::deleteDataInMap(int clientSocket)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
		return;
	it->second.requestData.clear();
}

bool hasCgiExtension(const std::string& filePath)
{
    size_t dotPosition = filePath.find_last_of('.');
    return dotPosition != std::string::npos && (filePath.substr(dotPosition) == ".py");
}

bool isDirectory(const std::string& path)
{
    struct stat fileInfo;
    
    if (stat(path.c_str(), &fileInfo) != 0)
	{
        std::cerr << "Error getting file information for " << path << std::endl;
        return false;
    }

    return S_ISDIR(fileInfo.st_mode);
}

void WebServerProg::sendResponse(int clientSocket)
{
	char method = accessDataInMap(clientSocket, "Method")[0];
	
	if (_status >= ERRORS) 
	{
		char buffer[1024] = {};
		std::string path = chooseErrorPage(_status);
		path = getcwd(buffer, sizeof(buffer)) + path;
		std::string body = readFile(path);

		appendStatus(_response, _status);
		appendBody(_response, body, path);
	}
	else if (isDirectory(accessDataInMap(clientSocket, "Path")))
	{
		std::cout << COLOR_GREEN << "Location is a directory" << COLOR_RESET << std::endl;
		_response.append(createDirectoryListing(accessDataInMap(clientSocket, "Path")));
	}
    else if (hasCgiExtension(accessDataInMap(clientSocket, "Path")))
	{
		CgiHandler cgi(m_clientDataMap.find(clientSocket)->second.requestData);
		appendStatus(_response, OK);
		_response.append(cgi.runCgi(accessDataInMap(clientSocket, "Path"), _request));
    }
	else
	{	
		switch (method) {
		case GET:
			getResponse(clientSocket);
			break;
		case POST:
			postResponse(clientSocket);
			break;
		case DELETE:
			deleteResponse(clientSocket);
			break;
		default:
			break;
		}
	}
	int bytes_sent = send(clientSocket, _response.c_str(), _response.size(), 0);
	if (bytes_sent < 0)
	{
		std::cout << "Error! send" << "\n";
		exit(EXIT_FAILURE);
	}
	deleteDataInMap(clientSocket);
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
		if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
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
			return ;
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
        if (errno == EAGAIN || errno == EWOULDBLOCK) //TODO:does this need to be removed
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
		int pollResult = poll(m_pollSocketsVec.data(), m_pollSocketsVec.size(), 1000);
		if (pollResult < 0)
		{
			std::cerr << "Error! poll" << std::endl;
			return ;
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
					initClientData(m_pollSocketsVec.back().fd, i);
					std::cout << "New connection accepted on client socket" << std::endl;
				}
				else
				{
						int check = receiveRequest(m_pollSocketsVec[i].fd, i);
						if (check)
						{
							if (check == 2)
								return;
							continue;
						}
						if (m_pollSocketsVec[i].revents & POLLOUT)
						{
							sendResponse(m_pollSocketsVec[i].fd);
							_request.clear();
							_status = NOT_SET;
							currentBodySize = 0;
							expectedBodySize = 0;
						}
				}
			}
		}
	}
}

void WebServerProg::startProgram()
{
	try
	{
		servers = parseConfigFile(configFileName);
		std::cout << COLOR_GREEN << "servers parsed" << COLOR_RESET << std::endl;
		validateServers(servers);
		initServers();
		runPoll();
	}
	catch (const std::exception& e)
	{
		std::cout << COLOR_RED << "Error! " << e.what() << "Server cannot start" << COLOR_RESET << std::endl;
		return ;
	}
}


WebServerProg::WebServerProg() : serverCount(0)
{	
	configFileName = "config/DefaultConfig.conf";
}

WebServerProg::WebServerProg(std::string fileName) : serverCount(0) , configFileName(fileName)
{}

WebServerProg::~WebServerProg()
{
}
