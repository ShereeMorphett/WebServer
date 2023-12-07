#include "../WebServerProg.hpp"
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <stdlib.h>

template <typename KeyType, typename ValueType>
void printMultimap(const std::multimap<KeyType, ValueType>& myMultimap) {
    typename std::multimap<KeyType, ValueType>::const_iterator it;

    for (it = myMultimap.begin(); it != myMultimap.end(); ++it) {
        std::cout << "Key: " << it->first << " | Value: " << it->second << std::endl;
    }
}

void WebServerProg::parseRequest(int clientSocket, std::string request)
{
	std::multimap<std::string, std::string>& clientRequestMap = m_clientDataMap.find(clientSocket)->second.requestData;
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

bool WebServerProg::receiveRequest(int clientSocket)
{
	if (m_clientDataMap.find(clientSocket) == m_clientDataMap.end())
	{
		clientData data;
		m_clientDataMap.insert(std::make_pair(clientSocket, data));
	}
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
