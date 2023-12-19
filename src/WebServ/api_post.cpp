#include "WebServerProg.hpp"
#include "constants.hpp"
#include "../Color.hpp"

void	WebServerProg::postResponse(int clientSocket) {
	// int	status = CREATED;

	// TODO: check for permissions
	std::string 	body = accessDataInMap(clientSocket, "Body"); //cgi query string is breaking here, returning null
		std::cout << COLOR_RED << "|" << body << "|" << COLOR_RESET << std::endl;
	if (!body.empty())
	{
		std::cout << COLOR_RED << body << COLOR_RESET << std::endl;
		std::ofstream	outFile("test.txt", std::ios::binary);
		if (!outFile) {
			// status = INT_ERROR;
			std::cerr << "Error: postResponse\n";
			return;
		}
		outFile << body;
		outFile.close();
	}
	else
		std::cerr << "NOT SURE WHAT HAPPENS HERE?\n";
}