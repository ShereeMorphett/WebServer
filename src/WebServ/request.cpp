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
#include <iterator>


server& WebServerProg::getClientServer(int clientSocket)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
	{
		;//!throw?
	}
	return servers[it->second.serverIndex];
}

clientData& WebServerProg::accessClientData(int clientSocket)
{
	return m_clientDataMap.find(clientSocket)->second;
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
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->root + path)); //TODO: sheree removed '/' if it breaks something
	
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
	if (clientRequestMap.find("requestPath")->second == "/src")
		return true;
	
	for (const auto& location : getClientServer(clientSocket).locations)
	{
		if (location.locationPath == clientRequestMap.find("requestPath")->second)
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


void WebServerProg::parseHeaders(int clientSocket, std::string requestChunk)
{
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
		return;

	std::multimap<std::string, std::string>& clientRequestMap = it->second.requestData;
	std::istringstream	requestStream(requestChunk);
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
	requestStream.ignore();
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
	accessClientData(clientSocket)._statusClient = IN_BODY;
	// Set rest of the chunk stream to _body of clientData
	if (accessDataInMap(clientSocket, "Method") != "POST")
		return;
	
	std::string body;
	body.assign(std::istreambuf_iterator<char>(requestStream), std::istreambuf_iterator<char>());
	clientRequestMap.insert(std::make_pair("Body", body));
	accessClientData(clientSocket).expectedBodySize = std::stoi(accessDataInMap(clientSocket, "Content-Length"));
	accessClientData(clientSocket).currentBodySize = accessDataInMap(clientSocket, "Body").size();

}

void WebServerProg::handleBody(int clientSocket, __attribute__((unused))std::string requestChunk)
{
	accessDataInMap(clientSocket, "Body").append(requestChunk);
	accessClientData(clientSocket).currentBodySize = accessDataInMap(clientSocket, "Body").size();
}

void WebServerProg::handleChunk(int clientSocket, std::string requestChunk)
{
	switch (accessClientData(clientSocket)._statusClient)
	{
		case NONE:
			parseHeaders(clientSocket, requestChunk);
			break;
		case IN_BODY:
			handleBody(clientSocket, requestChunk);
			break;
	}
}

bool WebServerProg::receiveRequest(int clientSocket, int pollIndex)
{
	char buffer[16384] = {};

	_request.clear();

	int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) //TODO: read and write check from revents
			return 1;
		std::cerr << "Error! recv" << std::endl;
		return 2;
	}
	else if (bytes_received == 0)
	{
		closeClientConnection(clientSocket);
		return 1;
	}
	else
	{
		std::string requestChunk(buffer, buffer + bytes_received);
		_request = buffer;
		std::cout << _request << std::endl;
		handleChunk(clientSocket, requestChunk);
	}
	if (accessClientData(clientSocket).currentBodySize == accessClientData(clientSocket).expectedBodySize)
	{
		m_pollSocketsVec[pollIndex].revents = POLLOUT;
	}
	return 0;
}
