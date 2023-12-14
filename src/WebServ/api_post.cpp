#include "WebServerProg.hpp"

void	WebServerProg::postResponse(int clientSocket) {
	// int	status = 201;
	std::string	body;

	body = accessDataInMap(clientSocket, "Body");
	std::cout << body << std::endl;
}