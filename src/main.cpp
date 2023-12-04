#include <sys/socket.h>
#include <sys/poll.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// #include "./WebServ/WebServ.hpp"
#include "./WebServ/WebServerProg.hpp"


int main ()
{
	//if no config file is given use default

	/*
	- When conducting load tests using the siege command be carefull it depend of your OS),
	it is crucial to limit the number of connections per second by specifying options such as -c (number of clients), -d (maximum wait time before a client reconnects), and -r (number of attempts). 
	*/
	WebServerProg program; // would take a validated config file at construction
	program.startProgram();
}