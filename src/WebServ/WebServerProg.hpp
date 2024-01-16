#ifndef WEBSERVERPROG_H
#define WEBSERVERPROG_H

# include <iostream>
# include <fstream>
# include <vector>
# include <map>
# include <poll.h>

struct location
{
    std::vector<std::string> allowedMethods;
    std::string locationPath;
    std::string redirection;
    std::string root;
    int listing;
    std::string defaultFile;
    std::string cgiPath;
};

struct server
{
	int port;
	int socketFD;
	std::string serverName;
    std::map<int, std::string> errorPages;
    std::vector<location> locations; 
    int clientMaxBodySize;

};

struct clientData
{
	int 			serverIndex;
	std::string 	headers;
	std::string 	body;
};


class WebServerProg
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;
		std::vector<server> servers;
		size_t serverCount;
		std::string defaultFileName;
		
		std::map<int, struct clientData> m_clientDataMap;
		std::string	_response;
		std::string	_request; 
		size_t		bodySize; 

	public:

		void addSocketToPoll(int socket, int event);
		void initClientData(int clientSocket, int serverIndex);
		void startProgram();
		void initServers();
		void sendResponse(int clientSocket);


		// std::string accessDataInMap(int clientSocket, std::string header);

		int receiveRequest(int clientSocket, int pollIndex);
		clientData&	getClientData(int clientSocket);
		std::string extractHeader(int clientSocket, std::string header);

		// void deleteDataInMap(int clientSocket);
		int  acceptConnection(int listenSocket);
		void runPoll();
		server& getClientServer(int clientSocket);

		WebServerProg();
		WebServerProg(std::string fileName);
		~WebServerProg();
		// void	deleteResponse(int clientSocket);
		void	postResponse(int clientSocket);
		void	getResponse(int clientSocket);
		void	deleteResponse(int clientSocket);
		bool 	validateRequest(int clientSocket, std::multimap<std::string, std::string>& clientRequestMap);

};

std::vector<struct server> parseConfigFile(const std::string& fileName);
void validateServers(const std::vector<struct server> &servers);

std::string runCgi();

#endif