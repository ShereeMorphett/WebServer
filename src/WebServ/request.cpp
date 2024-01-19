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

static int	countDepth(std::string path)
{
	int	depth = 0;

	for (size_t i = 0; i < path.size(); i++)
	{
		if (path[i] == '/')
			depth++;
	}

	return depth;
}

static bool	isFile(std::string path)
{
	if (path.find('.') != std::string::npos)
		return true;
	return false;
}

static void createPath(server& server, std::multimap<std::string, std::string>& clientRequestMap, std::string path)
{
	int	depth = countDepth(path);
	if (depth > ROOT)
	{
		std::string newPath = path.substr(0, path.find_first_of('/', 1));
		clientRequestMap.insert(std::make_pair("requestPath", newPath));
	}
	else
		clientRequestMap.insert(std::make_pair("requestPath", "/"));
	
	for (std::vector<location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
	{	
		if (path == it->locationPath)
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->root + '/' + it->defaultFile));
		}
		else if (depth <= ROOT && isFile(path))
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->root + '/' + path));
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
		std::cout << location.locationPath << " == " << clientRequestMap.find("requestPath")->second << std::endl;
		if (location.locationPath == clientRequestMap.find("requestPath")->second)
		{
			std::cout << COLOR_GREEN << "Found location" << COLOR_RESET << std::endl;
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
	if (!validateRequest(clientSocket, clientRequestMap))
	{
		std::cerr << COLOR_RED << "Method not allowed\n" << COLOR_RESET;
		_status = NOT_ALLOWED;
	}
	else if (clientRequestMap.find("Content-Length") != clientRequestMap.end())
	{
		char buffer[16384] = {};
		std::istringstream bodyLengthStream(clientRequestMap.find("Content-Length")->second);
		int bodyLength;
		if (!(bodyLengthStream >> bodyLength))
			std::runtime_error("Request parsing error!");
		expectedBodySize = bodyLength;

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
		currentBodySize += clientRequestMap.find("Body")->second.size();
	}
}

bool WebServerProg::receiveRequest(int clientSocket, int pollIndex)
{
	char buffer[8192] = {};

	_request.clear();
	int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
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
		// std::cout << COLOR_BLUE << "Request: " << "\n";
		// std::cout << COLOR_RED << _request << COLOR_RESET << std::endl;
		parseRequest(clientSocket, request);
	}
	if (currentBodySize == expectedBodySize) 
		m_pollSocketsVec[pollIndex].revents = POLLOUT;

	return 0;
}
