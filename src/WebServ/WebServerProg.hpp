

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
    int client_max_body_size;
    std::string serverName;
    std::map<int, std::string> errorPages; //this might be over kill
    std::vector<location> locations; 
};


class WebServerProg
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;// this is where the polling will happen
		//parse the configfile
		std::vector<struct server> servers;
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
void parseConfigFile(const char* fileName);

#endif