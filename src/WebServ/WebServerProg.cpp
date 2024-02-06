
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
#include <chrono>



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
	server&	serv = servers[serverIndex];
	clientData data(serv);

	data.location = nullptr;
	data.serverIndex = serverIndex;
	data.connectionTime = std::chrono::steady_clock::now();
	data._statusClient = NONE;
	data._status = NOT_SET;
	data._currentBodySize = 0;
	data._expectedBodySize = 0;
	data._requestReady = false;
	m_clientDataMap.insert(std::make_pair(clientSocket, data));
}

std::string WebServerProg::accessDataInMap(int clientSocket, std::string header)
{
    std::map<int, clientData>::iterator clientIt = m_clientDataMap.find(clientSocket);

    if (clientIt != m_clientDataMap.end())
    {
        auto& requestDataMap = clientIt->second.requestData;
        for (auto headerIt = requestDataMap.rbegin(); headerIt != requestDataMap.rend(); ++headerIt)
        {
            if (headerIt->first == header)
            {
                return headerIt->second; 
            }
        }
    }
    return "";
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

void WebServerProg::sendResponse(int clientSocket)
{
	clientData& client = accessClientData(clientSocket);

	char method = accessDataInMap(clientSocket, "Method")[0];
	std::string& response = accessClientData(clientSocket)._response;

	if (client.location && client.location->redirection == true)
	{
		std::string redirHeader = createRedirHeader(client);
		appendStatus(response, client.location->redirStatus);
		response.append(redirHeader);
		appendMisc(response, 0);
	}
	else if ((client.location && client.location->listing == true) && method == GET)
	{
		std::string myNewAwesomePath = client._root + client._requestPath;

		if (!isValidFile(myNewAwesomePath)) {
			response.append(createDirectoryListing(clientSocket, myNewAwesomePath));
		}
		else {
			replaceMapValue("Path", myNewAwesomePath, client);
			getResponse(clientSocket);
		}
	}
    else if (hasCgiExtension(accessDataInMap(clientSocket, "Path")))
	{
		CgiHandler cgi(m_clientDataMap.find(clientSocket)->second.requestData);
		appendStatus(response, OK);
		response.append(cgi.runCgi(accessDataInMap(clientSocket, "Path"), accessClientData(clientSocket)._requestClient));
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

	if (client._status >= ERRORS)
	{
		char buffer[1024] = {};
		std::string path = chooseErrorPage(client._status);
		path = getcwd(buffer, sizeof(buffer)) + path;
		std::string body = readFile(path);

		appendStatus(response, client._status);
		appendBody(response, body, path);
	}
  
	int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	if (bytes_sent < 0)
	{
		std::cout << "Error! send" << "\n";
	}
	response.clear();
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

int WebServerProg::acceptConnection(int listenSocket, int serverIndex)
{
    int clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket < 0)
    {
        errnoPrinting("Accept", errno);
        return -1; 
    }

	int maxBufferSize = BUFFER_SIZE;
	if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &maxBufferSize, sizeof(maxBufferSize)) == -1) 
    {
        errnoPrinting("setsockopt", errno);
        close(clientSocket);
        return -1;
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC))
    {
        errnoPrinting("fcntl", errno);
        close(clientSocket);  
        return -1;
    }
    addSocketToPoll(clientSocket, POLLIN);
	initClientData(clientSocket, serverIndex);
	
    return clientSocket;
}		
void	WebServerProg::closeClientConnection(int clientIndex)
{

	if (clientIndex >= 0 && clientIndex < static_cast<int>(m_pollSocketsVec.size()))
	{
		close(m_pollSocketsVec[clientIndex].fd);		
		m_clientDataMap.erase(m_pollSocketsVec[clientIndex].fd);
		m_pollSocketsVec.erase(m_pollSocketsVec.begin() + clientIndex);
	} 

}

static void	clearClientData(clientData& client)
{
	auto range = client.requestData.equal_range("requestPath");
	client.requestData.erase(range.first, range.second);
	range = client.requestData.equal_range("Path");
	client.requestData.erase(range.first, range.second);

	client.location = nullptr;

	client._statusClient = NONE;
	client._status = NOT_SET;

	client._expectedBodySize = 0;
	client._currentBodySize = 0;

	client._requestClient.clear();
	client._requestReady = false;
	client._requestPath.clear();

	client._bodyString.clear();
	client._rawRequest.clear();
	client._fileData.clear();
	client._response.clear();
	client._fileName.clear();

	client.connectionTime = std::chrono::steady_clock::now();
}

void WebServerProg::handleRequestResponse(int clientIndex)
{
	int check = receiveRequest(m_pollSocketsVec[clientIndex].fd, clientIndex);
	if (check)
	{
		return;
	}
	if (m_pollSocketsVec[clientIndex].revents & POLLOUT)
	{

		sendResponse(m_pollSocketsVec[clientIndex].fd);
		if (accessDataInMap(m_pollSocketsVec[clientIndex].fd,  "Connection") == "close")
		{
			closeClientConnection(clientIndex);
		}
		// TODO: If its keep alive, what should we reset?
		else
		{
			int	clientSocket = m_pollSocketsVec[clientIndex].fd;
			clientData& client = accessClientData(clientSocket);
			clearClientData(client);
		}
	}
}

void WebServerProg::handleEvents()
{
	for (size_t i  = 0; i < m_pollSocketsVec.size(); i++)
	{
		if (m_pollSocketsVec[i].revents & POLLIN)
		{
			if (i < serverCount)
			{
				acceptConnection(m_pollSocketsVec[i].fd, i);
			}
			else
			{
				handleRequestResponse(i);
			}
		}
	}
}

void WebServerProg::checkClientTimeout()
{
	for (size_t i  = 0; i < m_pollSocketsVec.size(); i++)
	{
		int socketFd = m_pollSocketsVec[i].fd;
		if (i >= serverCount)
		{
			auto client = m_clientDataMap.find(socketFd);
			std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<double> duration =  currentTime - client->second.connectionTime;
			if (duration > std::chrono::milliseconds(10))
			{
				closeClientConnection(i);
			}
		}
	}
}

void WebServerProg::runPoll()
{
	while (true)
	{
		checkClientTimeout();
		int pollResult = poll(m_pollSocketsVec.data(), m_pollSocketsVec.size(), 1000);
		if (pollResult < 0)
		{
			std::cerr << "Error! poll" << std::endl;
			return ;
		}	 
		if (pollResult == 0)
			continue;
		handleEvents();
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
		std::cout << COLOR_RED << "Error! " << e.what() << "\nServer cannot start" << COLOR_RESET << std::endl;
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
