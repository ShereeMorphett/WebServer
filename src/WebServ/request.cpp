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

// size = currentBodySize
void	WebServerProg::saveBody(int clientSocket, int size)
{
	std::string&			rawRequest = accessClientData(clientSocket)._rawRequest;
	std::vector<u_int8_t>	bodyVector;
	
	std::cout << "Current len: " << accessClientData(clientSocket)._currentBodySize << " + " << size << std::endl;
	rawRequest = accessClientData(clientSocket)._rawRequest;
	// std::cout << COLOR_MAGENTA << "raw: " << rawRequest << COLOR_RESET << std::endl;

	for (int i = 0; i < size; i++) {
		bodyVector.push_back(rawRequest[i]);
		std::cout << bodyVector[i];
	}

	std::string	content = std::string(bodyVector.begin(), bodyVector.end());
	accessClientData(clientSocket)._bodyString.append(content);
	accessClientData(clientSocket)._currentBodySize += size;
}

void WebServerProg::parseHeaders(int clientSocket, std::string requestChunk, int size)
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
	// Set rest of the chunk stream to _body of clientData

	if (accessDataInMap(clientSocket, "Method") != "POST")
		return;

	clientData& client = accessClientData(clientSocket);
	client._expectedBodySize = std::stoi(accessDataInMap(clientSocket, "Content-Length"));
	client._statusClient = IN_BODY;
	client._currentBodySize = 0;

	std::streampos position = requestStream.tellg();
	if (position != size) {	
		int	len = size - static_cast<int>(position);
		client._rawRequest = requestChunk.substr(len);
		saveBody(clientSocket, len);
	}

	// if (accessDataInMap(clientSocket, "Transfer-Encoding") == "chunked")
	// {
	// 	accessClientData(clientSocket)._statusClient = CHUNKED;
	// 	return;
	// }
}

static bool	checkValidBodySize(int clientSocket, clientData& client)
{
	if (client._currentBodySize != client._expectedBodySize)
		return false;
	// TODO: add cheks for max body size from headers and config

	return true;
}

void WebServerProg::handleBody(int clientSocket, std::string requestChunk, int size)
{
	clientData& client = accessClientData(clientSocket);

	client._rawRequest.append(requestChunk);
	saveBody(clientSocket, size);
	client._rawRequest.clear();

	if (checkValidBodySize(clientSocket, client)) {
		
	}
}

void WebServerProg::appendChunk(__attribute__((unused))int clientSocket, __attribute__((unused))std::string requestChunk)
{
	std::istringstream ss(requestChunk);

	size_t chunkSize;

	ss >> chunkSize;
	ss.ignore();
	ss.ignore();
	if (chunkSize == 0)
	{
		accessClientData(clientSocket)._statusClient = DONE;
		return;
	}
	std::string actualChunk;
	actualChunk.assign(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>());
	accessDataInMap(clientSocket, "Body").append(actualChunk);
}

void WebServerProg::handleChunk(int clientSocket, std::string requestChunk, int size)
{
	switch (accessClientData(clientSocket)._statusClient)
	{
		case NONE:
			parseHeaders(clientSocket, requestChunk, size);
			break;
		case IN_BODY:
			handleBody(clientSocket, requestChunk, size);
			break;
		case CHUNKED:
			appendChunk(clientSocket, requestChunk);
			break;
	}

}

bool WebServerProg::receiveRequest(int clientSocket, int pollIndex)
{
	char buffer[50000];

	std::memset(buffer, 0, 50000);
	int bytes_received = recv(clientSocket, buffer, 50000, 0);
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
		buffer[bytes_received] = '\0';
		std::string requestChunk(buffer, bytes_received);
		// std::cout << COLOR_YELLOW << "in recv: " << requestChunk << COLOR_RESET << std::endl;
		// accessClientData(clientSocket)._requestClient.append(buffer, buffer + bytes_received);
		// std::cout << COLOR_MAGENTA << "REGUEST:\n" << requestChunk << COLOR_RESET << std::endl;
		std::cout << "GOING TO HANDLE CHUNK: " << bytes_received << std::endl;;
		handleChunk(clientSocket, requestChunk, bytes_received);
	}
	std::cout << "curr: " << accessClientData(clientSocket)._currentBodySize << " exp: " << accessClientData(clientSocket)._expectedBodySize << std::endl;
	if (accessClientData(clientSocket)._statusClient != CHUNKED && accessClientData(clientSocket)._currentBodySize == accessClientData(clientSocket)._expectedBodySize)
	{
		std::cout << "POLLOUT\n";
		m_pollSocketsVec[pollIndex].revents = POLLOUT;
	}
	return 0;
}
