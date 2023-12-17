#include "WebServerProg.hpp"
#include "utils.hpp"
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <stdlib.h>
#include "../Color.hpp"

server& WebServerProg::getClientServer(int clientSocket)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
	{
		;//!throw?
	}
	return servers[it->second.serverIndex];
}


static void createPath(server& server, std::multimap<std::string, std::string>& clientRequestMap, std::string path)
{
	for (std::vector<location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
	{
		if (path == it->locationPath)
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->locationPath));
		}
		else if (it == server.locations.end() - 1)
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + path));
		}
	}
}

void WebServerProg::parseRequest(int clientSocket, std::string request)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
		return;

	std::multimap<std::string, std::string>& clientRequestMap = it->second.requestData;
	std::istringstream	ss(request);
	std::string			token;

	ss >> token;
	clientRequestMap.insert(std::make_pair("Method", token));
	ss >> token;
	createPath(getClientServer(clientSocket), clientRequestMap, token);
	ss >> token;
	clientRequestMap.insert(std::make_pair("HTTP-version", token));
	std::string line;
	while (std::getline(ss, line))
	{
		std::string key;
		std::string value;

		size_t pos = line.find(":");
		key = line.substr(0, pos);
		value = line.substr(pos + 1);

		size_t valueStart = value.find_first_not_of(" \t");
		value = value.substr(valueStart);

		if (key.size() != 1)
			clientRequestMap.insert(std::make_pair(key, value));
	}	
}

bool WebServerProg::receiveRequest(int clientSocket, int pollIndex)
{
	char buffer[1024];

	_request.clear();
	memset(buffer, 0, 1024);
	int bytes_received = recv(clientSocket, buffer, 1024, 0);

	
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return 1;
		}
		std::cout << "Error! recv" << std::endl;
		exit (1);
	}
	else if (bytes_received == 0)
	{
		std::cout << "Closing client socket" << std::endl;
		close(clientSocket);
		m_pollSocketsVec.erase(m_pollSocketsVec.begin() + pollIndex);
		m_clientDataMap.erase(m_clientDataMap.find(clientSocket));
		return 1;
	}
	else
	{
		std::string request = buffer;	
		_request = buffer;
		_request += '\0';
		parseRequest(clientSocket, request);
	}
	return 0;
}
