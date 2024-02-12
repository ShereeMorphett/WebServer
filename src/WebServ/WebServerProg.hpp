#ifndef WEBSERVERPROG_H
#define WEBSERVERPROG_H

# include <iostream>
# include <fstream>
# include <vector>
# include <map>
# include <poll.h>
# include <chrono>

struct Location
{
    std::vector<std::string> allowedMethods;

	std::string	redirLocation;
	bool		redirection;
	int			redirStatus;
	
    std::string	locationPath;
    std::string	root;
    bool 		listing;
    std::string	defaultFile;
    std::string	cgiPath;
	std::string alias;
};

struct Server
{
	int port;
	int socketFD;
	std::string serverName;
    std::map<int, std::string> errorPages;
    std::vector<Location> locations; 
    double clientMaxBodySize;
	std::string uploadDirectory;
};

struct clientData
{
	clientData(Server& serv) : server(serv) {}

	Location								*location;
	Server									&server;

	int 									serverIndex;
	std::multimap<std::string, std::string> requestData;
	std::chrono::steady_clock::time_point	connectionTime;
	std::string								_requestClient;
	int										_statusClient;

	int										_expectedBodySize;
	int										_currentBodySize; 
	std::string								_currentDirectory;
	std::string								_requestPath;
	std::string								_rawRequest;
	std::string								_root;

	std::string								_bodyString;
	std::string								_fileName;
	std::string								_fileData;

	int										_status;
	std::string								_response;
	bool									_requestReady;
};

class WebServerProg
{
	private:
		size_t 								serverCount;
		std::string 						configFileName;
		std::vector<struct pollfd> 			m_pollSocketsVec;
		std::map<int, struct clientData>	m_clientDataMap;
		std::vector<Server> 				servers;

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
		void handleChunk(int clientSocket, std::string request, int size);
		void handleBody(int clientSocket, std::string request, int size);
		void parseHeaders(int clientSocket, std::string requestChunk, int size);
		Server& getClientServer(int clientSocket);
		clientData& accessClientData(int clientSocket);
		void removeChunkSizes(int clientSocket);
		void appendChunk(int clientSocket, std::string requestChunk);

		void	saveBody(int clientSocket, int size);
		void	parseBody(int clientSocket);

		void handleEvents();
		void handleRequest(int clientIndex);
		void handleResponse(int clientIndex);
		int  acceptConnection(int listenSocket, int serverIndex);
		void checkClientTimeout();

		WebServerProg();
		WebServerProg(std::string fileName);
		~WebServerProg();
		void	postResponse(int clientSocket);
		void	getResponse(int clientSocket);
		void	deleteResponse(int clientSocket);
		bool 	validateRequest(int clientSocket, std::multimap<std::string, std::string>& clientRequestMap);
		std::string createDirectoryListing(int clientSocket, std::string startingPath);
		void	closeClientConnection(int clientIndex);
		void	confirmServerPort(int clientSocket);
};

std::vector<struct Server> parseConfigFile(const std::string& fileName);
void validateServers(const std::vector<struct Server> &servers);

std::string runCgi();

#endif