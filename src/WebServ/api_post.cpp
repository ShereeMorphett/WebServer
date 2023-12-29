#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"

// Cases to consider:
// malicious filename
// already exists

static void	appendMisc(std::string& _res) {
	_res.append("Content-Type: text/plain");
	_res.append(NEW_VALUE);
	_res.append("Content-length: 0");
	_res.append(END_HEADER);
}

void	WebServerProg::postResponse(int clientSocket) {
	int	status = OK;

	std::string 	body = accessDataInMap(clientSocket, "Body");
	std::ofstream	outFile("test.txt", std::ios::binary);
	if (!outFile) {
		status = INT_ERROR;
		std::cerr << "Error: ofstream\n";
		return;
	}
	outFile << body;
	outFile.close();

	appendStatus(_response, status);
	appendMisc(_response);
}