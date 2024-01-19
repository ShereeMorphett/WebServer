#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "Color.hpp"
#include <unistd.h>

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
	std::string 	body = accessDataInMap(clientSocket, "Body");
	std::string		fileName = fetchName(body);
	if (fileName == "error") {
		// Means filename was not provided by headers
		std::cout << COLOR_RED << "Error: filename not provided\n" << COLOR_RESET;
		return ;
	}

	std::ofstream	outFile(fileName, std::ios::binary);
	if (!outFile) {
		_status = INT_ERROR;
		std::cerr << "Error: ofstream\n";
		return;
	}
	outFile << body;
	outFile.close();
	
	_status = OK;
	appendStatus(_response, _status);
	appendMisc(_response);
}