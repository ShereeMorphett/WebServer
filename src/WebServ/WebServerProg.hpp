#ifndef WEBSERVERPROG_H
#define WEBSERVERPROG_H

# include <iostream>
# include <fstream>
# include <vector>
# include <map>
# include <poll.h>
# include <chrono>

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
	int 									serverIndex;
	std::multimap<std::string, std::string> requestData;
	std::chrono::steady_clock::time_point	connectionTime;
	std::string	_responseClient;
	std::string	_requestClient;
	size_t		currentBodySize; 
	size_t		expectedBodySize;
	int			_statusClient;
};

class WebServerProg
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;
		std::vector<server> servers;
		size_t serverCount;
		std::string configFileName;
		std::map<int, struct clientData> m_clientDataMap;
		std::string	_response;
		std::string	_request;
		int			_status;
		size_t		currentBodySize; 
		size_t		expectedBodySize;

	public:

		void addSocketToPoll(int socket, int event);
		void startProgram();
		void initServers();
		void sendResponse(int clientSocket);
		int  acceptConnection(int listenSocket);
		void runPoll();


		// Request methods
		bool receiveRequest(int clientSocket, int pollIndex);
		void initClientData(int clientSocket, int serverIndex);
		std::string accessDataInMap(int clientSocket, std::string header);
		void deleteDataInMap(int clientSocket);
		void handleChunk(int clientSocket, std::string request);
		void handleBody(int clientSocket, std::string request);
		void parseHeaders(int clientSocket, std::string requestChunk);
		server& getClientServer(int clientSocket);
		clientData& accessClientData(int clientSocket);


		void handleEvents();
		void handleRequestResponse(int clientIndex);
		int  acceptConnection(int listenSocket, int serverIndex);
		void checkClientTimeout();

		WebServerProg();
		WebServerProg(std::string fileName);
		~WebServerProg();
		void	postResponse(int clientSocket);
		void	getResponse(int clientSocket);
		void	deleteResponse(int clientSocket);
		bool 	validateRequest(int clientSocket, std::multimap<std::string, std::string>& clientRequestMap);
		std::string createDirectoryListing(std::string path, std::string referer);
		void	closeClientConnection(int clientIndex);
};

std::vector<struct server> parseConfigFile(const std::string& fileName);
void validateServers(const std::vector<struct server> &servers);

std::string runCgi();

#endif