#include "WebServerProg.hpp"
#include "utils.hpp"
#include "Color.hpp"
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <stdlib.h>
#include "constants.hpp"

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

bool WebServerProg::validateRequest(int clientSocket, std::multimap<std::string, std::string>& clientRequestMap)
{
	for (const auto& location : getClientServer(clientSocket).locations)
	{
		if (location.locationPath == clientRequestMap.find("Path")->second)
		{
			for (const auto& methods : location.allowedMethods)
			{
					if (methods == clientRequestMap.find("Method")->second)
					return true;
			}
		}
	}
	return false;
}

void WebServerProg::parseRequest(int clientSocket, std::string request)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
		return;

	std::multimap<std::string, std::string>& clientRequestMap = it->second.requestData;
	std::istringstream	requestStream(request);
	std::string			token;

	if (!(requestStream >> token))
		std::runtime_error("Request parsing error!");
	clientRequestMap.insert(std::make_pair("Method", token));
	if (!(requestStream >> token))
		std::runtime_error("Request parsing error!");
	createPath(getClientServer(clientSocket), clientRequestMap, token);

	if (!(requestStream >> token))
		std::runtime_error("Request parsing error!");
	clientRequestMap.insert(std::make_pair("HTTP-version", token));
	std::string line;
	requestStream.ignore();
	requestStream.ignore();// Ignore /r and /n
	while (std::getline(requestStream, line, '\r'))
	{
		std::string key;
		std::string value;

		size_t pos = line.find(":");
		if (pos == std::string::npos)
			std::runtime_error("Request parsing error!");
		key = line.substr(0, pos);
		value = line.substr(pos + 1);

            size_t valueStart = value.find_first_not_of(" \t");
            value = value.substr(valueStart);

		if (key.size() != 1)
			clientRequestMap.insert(std::make_pair(key, value));

		if (requestStream.peek() == '\n')
		{
			requestStream.ignore();
			if (requestStream.peek() == '\r')
			{
				requestStream.ignore();
				requestStream.ignore();
				break;
			}
		}
	}
	if (validateRequest(clientSocket, clientRequestMap) == false)
		std::runtime_error("Invalid request error!");

	if (clientRequestMap.find("Content-Length") != clientRequestMap.end())
	{
		char buffer[16384] = {};
		std::istringstream bodyLengthStream(clientRequestMap.find("Content-Length")->second);
		int bodyLength;
		if (!(bodyLengthStream >> bodyLength))
			std::runtime_error("Request parsing error!");
		bodySize = bodyLength;

		std::ostringstream bodyStream;
		while (bodyLength > 0)
		{
			int bytesRead = requestStream.readsome(buffer, std::min(static_cast<std::streamsize>(sizeof(buffer)), static_cast<std::streamsize>(bodyLength)));
			std::cout << COLOR_GREEN << "bytesRead:	" << bytesRead  << COLOR_RESET << std::endl;
			if (bytesRead <= 0)
			{
				std::runtime_error("Failed to read the entire body!");
				break;
			}
			bodyStream.write(buffer, bytesRead);
			bodyLength -= bytesRead;
		}
		std::string bodyStr = bodyStream.str();
		clientRequestMap.insert(std::make_pair("Body", bodyStr));
	}
	
	printMultimap(clientRequestMap);
}

bool WebServerProg::receiveRequest(int clientSocket, int pollIndex)
{
	char buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	int bytes_received = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 1;
		std::cout << "Error! recv" << std::endl;
		return 2;
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
		std::string request(buffer, buffer + bytes_received);
		_request = buffer;
		std::cout << COLOR_RED << "Request: " << "\n";
		std::cout << COLOR_RED << _request << COLOR_RESET << std::endl;
		parseRequest(clientSocket, request);
	}
	return 0;
}
