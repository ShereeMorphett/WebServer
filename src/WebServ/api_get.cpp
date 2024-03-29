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
		return;
	}
	body = readFile(path);

	appendStatus(client._response, client._status);
	appendBody(client._response, body, path);
}
