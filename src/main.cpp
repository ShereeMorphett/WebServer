#include <sys/socket.h>
#include <sys/poll.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./WebServ/WebServerProg.hpp"


int main(int argc, char** argv) 
{
	/*
	- When conducting load tests using the siege command be carefull it depend of your OS),
	it is crucial to limit the number of connections per second by specifying options such as -c (number of clients), -d (maximum wait time before a client reconnects), and -r (number of attempts). 
	*/
	if (argc == 1)
	{
		WebServerProg program; 
		program.startProgram();
	}
	else if (argc == 2)
	{
		WebServerProg program((std::string(argv[1])));

		program.startProgram();
	}
}