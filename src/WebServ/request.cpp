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
			std::string returnStr = clientHeaders.substr(pos, headerEnd - pos);
			return returnStr;
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
			if (extractHeader(clientSocket, "Method") == "POST")
			{
				getClientData(clientSocket).body = strBuffer.substr(pos + delimiter.length());
			}
			return REQUEAST_DONE;
			// std::cout << extractHeader(clientSocket, "Cache-Control") << std::endl;
			// exit(0);
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
