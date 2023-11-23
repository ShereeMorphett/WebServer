

#ifndef WEBSERVERPROG_H
#define WEBSERVERPROG_H

# include <iostream>
# include <vector>
# include <map>

struct location
{
    std::vector<int> allowedMethods;
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
	std::string serverName;
    std::map<int, std::string> errorPages; //this might be over kill
    std::vector<location> locations; 
    int clientMaxBodySize;
};


class WebServerProg
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;// this is where the polling will happen
		//parse the configfile
		std::vector<server> servers;
		size_t serverCount;
		const char *defaultFileName;
		
	public:

		void addSocketToPoll(int socket, int event);
		void startProgram();
		//parseConfig();
		int initServer(int port);
		void sendResponse(int clientSocket);
		void receiveRequest(int clientSocket);
		int acceptConnection(int listenSocket);
		void runPoll();
		WebServerProg();
		//WebServerProg(std::string filename);
		~WebServerProg();

};

//std::vector<struct server>
void parseConfigFile(const std::string& fileName);

#endif