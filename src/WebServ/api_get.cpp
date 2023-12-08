#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "Color.hpp"

// Function to append status code to response based on what readFile() returned
void	appendStatus(std::string& _res, int status) {
	_res.append(std::to_string(status)); // TODO will be changed to c++98 compitable
	switch (status) {
	case 200:
		_res.append(" OK");
		break;

	case 404:
		_res.append(" Not Found");
		break;

	default:
		break;
	}
	_res.append(NEW_VALUE);
}

// Append all other headers
void	appendMisc(std::string& _res) {
	_res.append("Connection: Closed");
	_res.append(NEW_VALUE);
}

// Append content type, length and actual body
void	appendBody(std::string& _res, std::string& body, std::string const & path) {
	_res.append("Content-Length: ");
	_res.append(std::to_string(body.size()));
	_res.append(NEW_VALUE);
	_res.append("Content-type: ");
	std::string type = getFileExtension(path);
	if (type == ".html") {
		_res.append(TYPE_HTML);
	}
	else if (type == ".css") {
		_res.append(TYPE_CSS);
	}
	_res.append(END_HEADER);
	_res.append(body);
}

// Check permissions and adjust status accordingly
void	checkPermissions(int* status) {
	// check method permissions
	if (*status) {
		return ;
	}
	// check path permissions

}

void	WebServerProg::getResponse(int clientSocket) {
	std::string body;
	std::string	path;
	int	status = 200;

	path = accessDataInMap(clientSocket, "Path");
	_response.append(HTTP_HEADER);
	_response.append(NEW_VALUE);
	checkPermissions(&status);
	if (status >= ERRORS) {
		path = chooseErrorPage(status);
	}
	body = readFile(path, &status);
	appendStatus(_response, status);
	appendMisc(_response);
	appendBody(_response, body, path);
}