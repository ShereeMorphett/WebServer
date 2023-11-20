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
	WebServ server;

	server.start();
}