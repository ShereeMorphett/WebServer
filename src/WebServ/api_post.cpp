#include "WebServerProg.hpp"
#include "constants.hpp"
#include "../Color.hpp"

void	WebServerProg::postResponse(int clientSocket) {
	// int	status = CREATED;

	// TODO: check for permissions

	std::string 	body = accessDataInMap(clientSocket, "Body");
	std::ofstream	outFile("test.png", std::ios::binary);

	if (!outFile) {
		// status = INT_ERROR;
		std::cerr << "Error: postResponse\n";
		return;
	}
	outFile << body;
	outFile.close();
}