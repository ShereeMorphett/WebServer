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
		;// TODO: !throw?
	}
	return servers[it->second.serverIndex];
}

clientData& WebServerProg::accessClientData(int clientSocket)
{
	return m_clientDataMap.find(clientSocket)->second;
}

static bool	isFile(std::string path)
{
	if (path.find('.') != std::string::npos)
		return true;
	return false;
}

static bool createPath(server& server, std::multimap<std::string, std::string>& clientRequestMap, std::string path, clientData& client)
{
	int	depth = countDepth(path);

	char c = path.back();
	if (c == '/' && depth > ROOT)
		path.pop_back();
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	for (std::vector<location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
	{	
		if (path == it->locationPath)
		{
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->root + '/' + it->defaultFile));
			clientRequestMap.insert(std::make_pair("requestPath", it->locationPath));
			break;
		}
		else if (depth <= ROOT && isFile(path))
		{
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->root + path));
			clientRequestMap.insert(std::make_pair("requestPath", "/"));
			break;
		}
		else if (it == server.locations.end() - 1)
		{
			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + path));
			clientRequestMap.insert(std::make_pair("requestPath", path.substr(0, path.find_first_of('/', 1))));
			break;
		}
	}

	client._root = buffer;
	return true;
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

void	WebServerProg::saveBody(int clientSocket, int size)
{
	std::string&			rawRequest = accessClientData(clientSocket)._rawRequest;
	std::vector<u_int8_t>	bodyVector;
	
	for (int i = 0; i < size; i++) {
		bodyVector.push_back(rawRequest[i]);
	}

	std::string	content = std::string(bodyVector.begin(), bodyVector.end());
	accessClientData(clientSocket)._bodyString.append(content);
	accessClientData(clientSocket)._currentBodySize += size;
}

static std::string	fetchName(std::string& body) {
	std::string	target = "filename=\""; 
	size_t	startPos = body.find(target);
	if (startPos == std::string::npos)
	{
		target = "name=\"";
		startPos = body.find(target);
		if (startPos == std::string::npos)
			return "error";
	}

	startPos += target.length();
	size_t	endPos = body.find("\"", startPos);
	if (endPos == std::string::npos)
		return "error";

	size_t len = endPos - startPos;
	std::string	name = body.substr(startPos, len);

	return name;
}

static std::string	parseBoundary(std::string& format)
{
	size_t		boundaryStart = format.find(" boundary=");
	std::string	boundary;


	if (boundaryStart == std::string::npos)
		return boundary;
	
	boundary = format.substr(boundaryStart + 10);
	return boundary;
}

static bool	removeBoundary(clientData& client, std::string& boundary)
{
	size_t start = client._bodyString.find("\r\n\r\n") + 4;
    size_t end = client._bodyString.rfind(boundary + "--");

    if (start != std::string::npos && end != std::string::npos) {
        client._fileData = client._bodyString.substr(start, end - start);
		return true;
	}
	
	return false;
}

// NOTE: Currently handles just multipart form 
void	WebServerProg::parseBody(int clientSocket)
{
	clientData& client = accessClientData(clientSocket);
	std::string	format = accessDataInMap(clientSocket, "Content-Type");

	if (format.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		
		client.requestData.insert(std::make_pair("Body", client._bodyString));
		client._requestReady = true;
		return;
	}
	if (format.find("multipart/form-data;") != std::string::npos)
	{
		client._fileName = fetchName(client._bodyString);
		std::string boundary = parseBoundary(format);
		
		if (boundary.empty()) {
			client._status = BAD_REQUEST;
			return;
		}

		if (!removeBoundary(client, boundary)) {
			client._status = BAD_REQUEST;
			return;
		}
		client._requestReady = true;
	}
	else
	{
		client._status = BAD_REQUEST;
		return;
	}
}
	// else
	// {
	// 	client._status = BAD_REQUEST;
	// }


static bool	checkValidBodySize(clientData& client)
{
	if (client._currentBodySize != client._expectedBodySize)
		return false;
	if (client._currentBodySize > client.server.clientMaxBodySize)
		return false;
	if (client._currentBodySize > client._expectedBodySize)
		return false;

	return true;
}

static bool	addRequestLocation(clientData& client, std::string const & path)
{
	for (size_t i = 0; i < client.server.locations.size(); i++)
	{
		if (client.server.locations[i].locationPath == path)
		{
			client.location = &client.server.locations[i];
			return true;
		}
	}

	return false;
}

void WebServerProg::parseHeaders(int clientSocket, std::string requestChunk, int size)
{
	char c;
	std::map<int, clientData>::iterator it = m_clientDataMap.find(clientSocket);
	if (it == m_clientDataMap.end())
		return;
	
	clientData& client = accessClientData(clientSocket);

	std::multimap<std::string, std::string>& clientRequestMap = it->second.requestData;
	std::istringstream	requestStream(requestChunk);
	std::string			token;

	if (!(requestStream >> token))
		throw std::runtime_error("Request parsing error!");
	clientRequestMap.insert(std::make_pair("Method", token));
	if (!(requestStream >> token))
		throw std::runtime_error("Request parsing error!");
	c = token.back();
	if (c == '/')
		token.pop_back();
	if (!createPath(getClientServer(clientSocket), clientRequestMap, token, client))
	{
		client._status = INT_ERROR;
		throw std::runtime_error("Request parsing error!");
	}
	if (!addRequestLocation(client, clientRequestMap.find("requestPath")->second))
	{
		client._status = INT_ERROR;
		throw std::runtime_error("Request parsing error!");
	}
	client._requestPath = token;
	if (!(requestStream >> token))
		throw std::runtime_error("Request parsing error!");
	clientRequestMap.insert(std::make_pair("HTTP-version", token));
	std::string line;
	requestStream.ignore();
	requestStream.ignore();

	while (std::getline(requestStream, line) && !line.empty())
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		size_t pos = line.find(":");
		if (pos == std::string::npos) {
			if (line.empty())
				break;
			else
			{
				throw std::runtime_error("Request parsing error\n");
				client._status = BAD_REQUEST;
			}
		}

		std::string	key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		size_t valueStart = value.find_first_not_of(" \t");
		value = value.substr(valueStart);

		if (key.size() > 0)
			clientRequestMap.insert(std::make_pair(key, value));
	}

	if (client._status < ERRORS && !validateRequest(clientSocket, clientRequestMap))
	{
		std::cerr << COLOR_RED << "Method not allowed\n" << COLOR_RESET;
		client._status = NOT_ALLOWED;
		client._requestReady = true;
		return;
	}
	// Set rest of the chunk stream to _body of clientData
	
	if (accessDataInMap(clientSocket, "Method") != "POST") {
		client._requestReady = true;
		return;
	}

	client._expectedBodySize = std::stoi(accessDataInMap(clientSocket, "Content-Length"));
	client._statusClient = IN_BODY;
	client._currentBodySize = 0;
	
	std::streampos position = requestStream.tellg();
	int currentPos = static_cast<int>(position);
	int	len = size - currentPos;
	if (len > 0) {	
		client._rawRequest = requestChunk.substr(currentPos);
		saveBody(clientSocket, len);
		client._rawRequest.clear();

		if (checkValidBodySize(client)) {
			parseBody(clientSocket);
		}
	}

	// TODO: Do we need a chunked one? If so handle it here
	
	// if (accessDataInMap(clientSocket, "Transfer-Encoding") == "chunked")
	// {
	// 	accessClientData(clientSocket)._statusClient = CHUNKED;
	// 	return;
	// }
}



void WebServerProg::handleBody(int clientSocket, std::string requestChunk, int size)
{
	clientData& client = accessClientData(clientSocket);

	client._rawRequest.append(requestChunk);
	saveBody(clientSocket, size);
	client._rawRequest.clear();

	// TODO: Check correct status
	if (checkValidBodySize(client)) {
		parseBody(clientSocket);
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
	try {
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
	catch(std::exception &e) {
		accessClientData(clientSocket)._status = BAD_REQUEST;
		accessClientData(clientSocket)._requestReady = true;
	}

}

bool WebServerProg::receiveRequest(int clientSocket, int pollIndex)
{
	char buffer[50000];

	std::memset(buffer, 0, 50000);
	int bytes_received = recv(clientSocket, buffer, 50000, 0);
	if (bytes_received < 0)
	{
		if (m_pollSocketsVec[pollIndex].revents & (POLLIN | POLLRDNORM | POLLRDBAND)) // is not error, just waiting and try again after
            return true; 
        std::cerr << "Error! recv" << std::endl;
        return true;
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
		accessClientData(clientSocket)._requestClient.append(buffer, buffer + bytes_received);	
		handleChunk(clientSocket, requestChunk, bytes_received);
	}

	if (accessClientData(clientSocket)._statusClient != CHUNKED && accessClientData(clientSocket)._requestReady)
	{
		m_pollSocketsVec[pollIndex].revents = POLLOUT;
	}
	return 0;
}
