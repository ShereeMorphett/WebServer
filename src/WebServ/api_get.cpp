#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "utils.hpp"
#include <unistd.h>

void	WebServerProg::getResponse(int clientSocket) {
	std::string body;
	std::string	path;

	path = accessDataInMap(clientSocket, "Path");
	checkRequest(&_status, path);
	if (_status >= ERRORS) {
		char buffer[1024] = {};
		path = chooseErrorPage(_status);
		path = getcwd(buffer, sizeof(buffer)) + path;
	}
	body = readFile(path);

	appendStatus(_response, _status);
	appendBody(_response, body, path);
}
