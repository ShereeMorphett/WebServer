#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "Color.hpp"

static std::string	fetchName(std::string& body) {
	std::string	target = "filename=\"";

	size_t	startPos = body.find(target);
	if (startPos == std::string::npos) {
		return "error";
	}

	startPos += target.length();
	size_t	endPos = body.find("\"", startPos);
	if (endPos == std::string::npos) {
		return "error";
	}

	size_t len = endPos - startPos;
	std::string	name = body.substr(startPos, len);

	return name;
}

static void	appendMisc(std::string& _res) {
	_res.append("Content-Type: text/plain");
	_res.append(NEW_VALUE);
	_res.append("Content-length: 0");
	_res.append(END_HEADER);
}

void	WebServerProg::postResponse(int clientSocket) {
	int	status = OK;

	std::string 	body = accessDataInMap(clientSocket, "Body");
	std::string		fileName = fetchName(body);
	if (fileName == "error") {
		// Means filename was not provided by headers
	}

	std::ofstream	outFile(fileName, std::ios::binary);
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