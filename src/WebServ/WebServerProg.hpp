

#ifndef WEBSERVERPROG_H
#define WEBSERVERPROG_H

# include <iostream>
# include <vector>
# include <map>

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


class WebServerProg
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;
		std::vector<server> servers;
		size_t serverCount;
		std::string defaultFileName;
		
	public:

		void addSocketToPoll(int socket, int event);
		void startProgram();
		void initServers();
		void sendResponse(int clientSocket);
		void receiveRequest(int clientSocket);
		int acceptConnection(int listenSocket);
		void runPoll();
		WebServerProg();
		WebServerProg(std::string fileName);
		~WebServerProg();

};

std::vector<struct server> parseConfigFile(const std::string& fileName);
void validateServers(const std::vector<struct server> &servers);

#endif