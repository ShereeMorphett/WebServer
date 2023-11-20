#include <sys/socket.h>
#include <sys/poll.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./WebServ/WebServ.hpp"

int main ()
{
	//validate the config file from here
	WebServ server; // would take a validated config file at construction
	server.start();
}