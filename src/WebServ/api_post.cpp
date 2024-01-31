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

void parseAndWriteToFile(const std::string& body, const std::string& fileName) {
    // Find the start and end positions of the actual content
    size_t start = body.find("\r\n\r\n") + 4;
    size_t end = body.rfind("\r\n------");

    // Check if the boundaries are found
    if (start != std::string::npos && end != std::string::npos) {
        // Extract the relevant content
        std::string content = body.substr(start, end - start);

        // Write the content to the file
        std::ofstream outFile(fileName, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error: ofstream\n";
            return;
        }

        outFile << content;
        outFile.close();
    } else {
        std::cerr << "Error: Boundaries not found\n";
    }
}

void WebServerProg::postResponse(int clientSocket) {
	std::string&	response = accessClientData(clientSocket)._response;
    std::string		body = accessDataInMap(clientSocket, "Body");
    std::string		fileName = fetchName(body);

    if (fileName == "error") {
        // Means filename was not provided by headers
        std::cout << COLOR_RED << "Error: filename not provided\n" << COLOR_RESET;
        return;
    }

    parseAndWriteToFile(body, fileName);

    _status = OK;
    appendStatus(response, _status);
    appendMisc(response);
}