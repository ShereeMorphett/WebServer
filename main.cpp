
/* 
	this is a placeholder main for the socket handler class
*/
#include "SocketHandler.hpp"
#include <sys/socket.h>

int main ()
{

	SocketHandler sockets;
	
	sockets.createBindSocket(AF_INET, SOCK_STREAM, 0);

	return 0;
}
