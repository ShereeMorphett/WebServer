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

// static void createPath(server& server, std::multimap<std::string, std::string>& clientRequestMap, std::string path)
// {
// 	for (std::vector<location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
// 	{
// 		if (path == it->locationPath)
// 		{
// 			char buffer[1024];
// 			memset(buffer, 0, sizeof(buffer));
// 			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + it->locationPath));
// 		}
// 		else if (it == server.locations.end() - 1)
// 		{
// 			char buffer[1024];
// 			memset(buffer, 0, sizeof(buffer));
// 			clientRequestMap.insert(std::make_pair("Path", getcwd(buffer, sizeof(buffer)) + path));
// 		}
// 	}
// }

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

clientData&	WebServerProg::getClientData(int clientSocket)
{
	return m_clientDataMap.find(clientSocket)->second;
}

std::string WebServerProg::extractHeader(int clientSocket, std::string header)
{
	std::string clientHeaders = getClientData(clientSocket).headers;
	if (header == "Method")
	{
		size_t methodEnd = clientHeaders.find(' ');
		return clientHeaders.substr(0, methodEnd);
	}
	else if (header == "Path")
	{
		size_t pathStart = clientHeaders.find(' ') + 1;
		size_t pathEnd = clientHeaders.find(' ', pathStart);
		return clientHeaders.substr(pathStart, pathEnd - pathStart);
	}
	else if (header == "HTTP-version")
	{
		size_t versionStart = clientHeaders.find(' ') + 1;
		size_t versionEnd = clientHeaders.find('\r');
		versionStart = clientHeaders.find(' ', versionStart) + 1;
		return clientHeaders.substr(versionStart, versionEnd - versionStart);
	}
	else if (header == "Boundary")
	{
		size_t pos = clientHeaders.find("boundary=");
		if (pos != std::string::npos)
		{
			pos += header.length() + 1;
			size_t headerEnd = clientHeaders.find('\r', pos);
			return clientHeaders.substr(pos, headerEnd - pos);
		}
	}
	else
	{
		size_t pos = clientHeaders.find(header + ":");
		if (pos != std::string::npos)
		{
			pos += header.length() + 2;
			size_t headerEnd = clientHeaders.find('\r', pos);
			std::cout << header.length() << std::endl;
			std::string returnStr = clientHeaders.substr(pos, headerEnd - pos);
			std::cout << "'" << returnStr << "'" << std::endl;
			exit(0);
		}
	}
	return "";
}

int WebServerProg::receiveRequest(int clientSocket, __attribute__((unused))int pollIndex)
{
	char recvBuffer[8192] = {};

	_request.clear();

	std::string strBuffer;
	std::string headers;
	std::string body;
	int bytes_received;
	const std::string delimiter = "\r\n\r\n";

	bytes_received = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
	
	if (bytes_received < 0)
	{
		std::cout << "Error! recv" << std::endl;
		return REQUEST_ERROR;
	}

	else if (bytes_received == 0)
	{
		std::cout << "Closing client socket" << std::endl;
		close(clientSocket);
		m_pollSocketsVec.erase(m_pollSocketsVec.begin() + pollIndex);
		m_clientDataMap.erase(m_clientDataMap.find(clientSocket));
		return REQUEST_CLIENT_CLOSED;
	}
	else
	{
		strBuffer.append(recvBuffer, bytes_received);

		size_t pos = strBuffer.find(delimiter);
		if (pos != std::string::npos)
		{
			getClientData(clientSocket).headers = strBuffer.substr(0, pos);
			std::cout << extractHeader(clientSocket, "Content-Type") << std::endl;
			exit(0);
			// body = strBuffer.substr(pos + delimiter.length());
			// std::cout << headers << std::endl;
		}
	}
	return 0;

}



	// int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
	// if (bytes_received < 0)
	// {
	// 	if (errno == EAGAIN || errno == EWOULDBLOCK)
	// 		return 1;
	// 	std::cout << "Error! recv" << std::endl;
	// 	return REQUEST_ERROR;
	// }
	// else if (bytes_received == 0)
	// {
	// 	std::cout << "Closing client socket" << std::endl;
	// 	close(clientSocket);
	// 	m_pollSocketsVec.erase(m_pollSocketsVec.begin() + pollIndex);
	// 	m_clientDataMap.erase(m_clientDataMap.find(clientSocket));
	// 	return REQUEST_CLIENT_CLOSED;
	// }
	// else
	// {
	// 	std::string requestChunk(buffer, buffer + bytes_received);
	// 	_request = buffer;
	// 	parseRequest(clientSocket, requestChunk);
	// }
	// return 0;

	// int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
	// if (bytes_received < 0)
	// {
	// 	if (errno == EAGAIN || errno == EWOULDBLOCK)
	// 		return 1;
	// 	std::cout << "Error! recv" << std::endl;
	// 	return 2;
	// }
	// else if (bytes_received == 0)
	// {
	// 	std::cout << "Closing client socket" << std::endl;
	// 	close(clientSocket);
	// 	m_pollSocketsVec.erase(m_pollSocketsVec.begin() + pollIndex);
	// 	m_clientDataMap.erase(m_clientDataMap.find(clientSocket));
	// 	return 1;
	// }
	// else
	// {
	// 	std::string request(buffer, buffer + bytes_received);
	// 	_request = buffer;
	// 	// std::cout << COLOR_RED << "Request: " << "\n";
	// 	// std::cout << COLOR_RED << _request << COLOR_RESET << std::endl;
	// 	parseRequest(clientSocket, request);
	// }
	// return 0;
// }
