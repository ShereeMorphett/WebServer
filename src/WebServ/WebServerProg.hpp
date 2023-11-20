

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
		
	public:

		void addSocketToPoll(int socket, int event);
		void startProgram();
		//parseConfig();
		//initServers;
		int initServer();
		int initServer2(); //not staying
		void sendResponse(int clientSocket);
		void receiveRequest(int clientSocket);
		WebServerProg();
		//WebServerProg(std::string filename);
		~WebServerProg();

};


#endif