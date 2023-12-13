#include "WebServerProg.hpp"
#include "utils.hpp"
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <stdlib.h>

int WebServerProg::getClientServer(int clientSocket)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
		return 0;
	return it->second.serverIndex;	
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
	clientRequestMap.insert(std::make_pair("Path", token));
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
		parseRequest(clientSocket, request);
	}
	return 0;
}
