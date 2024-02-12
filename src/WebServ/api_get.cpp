#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "utils.hpp"
#include <unistd.h>

void	WebServerProg::getResponse(int clientSocket) {
	clientData& 	client = accessClientData(clientSocket);
	std::string 	body;
	std::string		path;

	path = accessDataInMap(clientSocket, "Path");
	checkRequest(&client._status, path);
	if (client._status >= ERRORS) {
		char buffer[1024] = {};
		path = chooseErrorPage(client);
		if (path.empty()) {
			appendStatus(client._response, client._status);
			appendMisc(client._response, 0);
			return;
		}
		path = getcwd(buffer, sizeof(buffer)) + path;
	}
	body = readFile(path);

	appendStatus(client._response, client._status);
	appendBody(client._response, body, path);
}
