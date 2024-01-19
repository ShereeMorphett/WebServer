#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "utils.hpp"
#include <unistd.h>

// Append content type, length and actual body
void	appendBody(std::string& _res, std::string& body, std::string const & path) {
	_res.append("Content-Length: ");
	_res.append(toString(body.size()));
 	_res.append(NEW_VALUE);
	_res.append("Content-type: ");
	std::string type = getFileExtension(path);
	if (type == EXT_HTML) {
		_res.append(TYPE_HTML);
	}
	else if (type == EXT_CSS) {
		_res.append(TYPE_CSS);
	}
	else if (type == EXT_PNG) {
		_res.append(TYPE_PNG);
	}
	_res.append(END_HEADER);
	_res.append(body);
}

// Check permissions and adjust status accordingly
void	checkRequest(int* status, std::string const & path) {
	std::ifstream	file(path.c_str());
	if (file.good()) {
		*status = OK;
	}
	else {
		*status = NOT_FOUND;
	}
}


static std::string createPath(server& server, std::string path)
{
	for (auto it = server.locations.begin(); it != server.locations.end(); it++)
	{
		if (path == it->locationPath)
		{
			char buffer[1024] = {};
			return getcwd(buffer, sizeof(buffer)) + it->locationPath;
		}
		else if (it == server.locations.end() - 1)
		{
			char buffer[1024] = {};
			return getcwd(buffer, sizeof(buffer)) + path;
		}
	}
	return "";
}

void	WebServerProg::getResponse(int clientSocket) {
	std::string body;
	std::string	path;
	int	status = 200;

	path = createPath(getClientServer(clientSocket), extractHeader(clientSocket, "Path"));
	std::cout << path << std::endl;
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
