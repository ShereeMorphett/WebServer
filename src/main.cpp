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
	//if 
	WebServerProg program; // would take a validated config file at construction
	program.startProgram();
}