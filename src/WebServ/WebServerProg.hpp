

#ifndef WebServerProg_HPP
# define WebServerProg_HPP

# include <iostream>
# include <vector>
# include <map>



class WebServerProg
{
	private:
		std::vector<struct pollfd> m_pollSocketsVec;// this is where the polling will happen
		//parse the configfile
		//vector<maps<string, string>> servers
		size_t serverCount;
		
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


#endif