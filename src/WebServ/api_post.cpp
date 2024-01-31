#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "Color.hpp"
#include <unistd.h>

static void	appendMisc(std::string& _res) {
	_res.append("Content-Type: text/plain");
	_res.append(NEW_VALUE);
	_res.append("Content-length: 0");
	_res.append(END_HEADER);
}

void WebServerProg::postResponse(int clientSocket) {
    clientData& client = accessClientData(clientSocket);

	if (client._status < ERRORS) {	
		std::ofstream outFile(client._fileName, std::ios::binary);
		if (!outFile)
			client._status = INT_ERROR;
		else {
			outFile << client._fileData;
			outFile.close();
			client._status = OK;
		}
	}
    
    appendStatus(client._response, client._status);
    appendMisc(client._response);
}