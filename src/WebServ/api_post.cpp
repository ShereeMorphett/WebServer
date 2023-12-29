#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"

void	WebServerProg::postResponse(int clientSocket) {
	int	status = OK;

	std::string 	body = accessDataInMap(clientSocket, "Body");
	std::ofstream	outFile("test.jpg", std::ios::binary);
	if (!outFile) {
		status = INT_ERROR;
		std::cerr << "Error: ofstream\n";
		return;
	}
	outFile << body;
	outFile.close();

	appendStatus(_response, status);
	_response.append(END_HEADER);
}