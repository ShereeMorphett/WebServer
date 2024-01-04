#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "utils.hpp"
#include <unistd.h>


void	WebServerProg::getResponse(int clientSocket) {
	std::string body;
	std::string	path;
	int	status = 200;

	path = accessDataInMap(clientSocket, "Path");
	checkRequest(&status, path);
	if (status >= ERRORS) {
		char buffer[1024] = {};
		path = chooseErrorPage(status);
		path = getcwd(buffer, sizeof(buffer)) + path;
	}
	body = readFile(path);

	appendStatus(_response, status);
	appendBody(_response, body, path);
}
