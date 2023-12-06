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

// Append content type, length and actual body
void	appendBody(std::string& _res, std::string& body, std::string const & path) {
	_res.append("Content-Length: ");
	_res.append(std::to_string(body.size()));
	_res.append(NEW_VALUE);
	_res.append("Content-type: ");
	std::string type = getFileExtension(path);
	std::cout << "type: " << type << "\n";
	if (type == ".html") {
		_res.append(TYPE_HTML);
	}
	else if (type == ".css") {
		_res.append(TYPE_CSS);
	}
	_res.append(END_HEADER);

	_res.append(body);
}

void	WebServerProg::getResponse(int clientSocket) {
	std::string body;
	std::string	path;
	int	status = 200;

	path = accessDataInMap(clientSocket, "Path");
	_response.append(HTTP_HEADER);
	body = readFile(path, &status);
	appendStatus(_response, status);

	// This might be sent anyways
	// TODO: check if this is required etc needed and in what scenarios
	_response.append("Connection: Closed");
	_response.append(NEW_VALUE);

	appendBody(_response, body, path);
}