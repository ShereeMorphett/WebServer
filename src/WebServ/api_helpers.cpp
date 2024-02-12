#include "utils.hpp"
#include "api_helpers.hpp"
#include "Color.hpp"
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

// Get path out of request string
std::string extractPath(std::string const & request)
{
    size_t firstSpace = request.find(' ');
    if (firstSpace == std::string::npos) {
        return "";
    }

    size_t secondSpace = request.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos) {
        return "";
    }

    return request.substr(firstSpace + 1, secondSpace - firstSpace - 1);
}

// gets file extension from request path, for example .html
std::string getFileExtension(std::string const & fileName) {
    size_t dotPosition = fileName.find_last_of(".");

    if (dotPosition != std::string::npos && dotPosition != fileName.length() - 1) {
        return fileName.substr(dotPosition + 1);
    } else {
		std::cout << "Invalid file, no extension \n";
        return "";
	}
}

// Will read a file based on path given
std::string	readFile(std::string const & path) {
	std::ifstream	inFile;
	inFile.open(path.c_str(), std::ios::binary | std::ios::ate);
	if (!inFile) {
		std::cerr << "Error with infile\n";
		return "";
	}
	std::streamsize	size = inFile.tellg();
	inFile.seekg(0, std::ios::beg);
	std::vector<char>	buffer(size);
	inFile.read(&buffer[0], size);
	inFile.close();

	std::string body(buffer.begin(), buffer.end());
	return body;
}

// std::string	chooseErrorPage(int status) {
// 	switch (status) {
// 		case FORBIDDEN:
// 			return FORBIDDEN_PAGE;
		
// 		case UNAUTHORIZED:
// 			return UNAUTHORIZED_PAGE;

// 		case NOT_FOUND:
// 			return NOT_FOUND_PAGE;

// 		case NOT_ALLOWED:
// 			return NOT_ALLOWED_PAGE;

// 		default:
// 			break; 
// 	}
// 	return SERVER_PAGE;
// }

std::string	chooseErrorPage(clientData& client) {
	std::string	path = "/defaults";

	if (client.server.errorPages.find(client._status)->second.empty()) {
		return "";
	}

	switch (client._status) {
		case NOT_FOUND:
			path.append(client.server.errorPages.find(NOT_FOUND)->second);
			break;

		case NOT_ALLOWED:
			path.append(client.server.errorPages.find(NOT_ALLOWED)->second);
			break;

		case INT_ERROR:
			path.append(client.server.errorPages.find(NOT_FOUND)->second);
			break;
		
		case 504:
			path.append(client.server.errorPages.find(504)->second);
			break;

		default:
			break; 
	}

	return path;
}

std::string	createRedirHeader(clientData& client)
{
	std::string newLocation = client.location->redirLocation;
	std::string	defaultFile;
	std::string	root;
	
	for (size_t i = 0; i < client.server.locations.size(); i++)
	{
		if (client.server.locations[i].locationPath == newLocation)
		{
			defaultFile = client.server.locations[i].defaultFile;
			root = client.server.locations[i].root;
		}
	}
	
	if (root.empty() || defaultFile.empty()) {
		client._status = INT_ERROR;
		return "";
	}

	std::string	header;
	header.append("Location: ");
	header.append(root + '/' + defaultFile);
	header.append(END_HEADER);

	return header;
}

void	appendStatus(std::string& _res, int status) {
	_res.append(HTTP_HEADER);
	_res.append(toString(status));
	switch (status) {
		case OK:
			_res.append(" OK");
			break;

		case NOT_FOUND:
			_res.append(" Not Found");
			break;

		case NOT_ALLOWED:
			_res.append(" Not Allowed");
			break;
		
		case ACCEPTED:
			_res.append(" Accepted");
			break;
		
		case NO_CONTENT:
			_res.append(" No Content");
			break;

		case BAD_REQUEST:
			_res.append(" Bad Request");
			break;

		case TEMP_REDIR:
			_res.append(" Temporary Redirect");
			break;

		case PERMA_REDIR:
			_res.append(" Permanent Redirect");
			break;
 
		default:
			break;
	}
	_res.append(NEW_VALUE);
  
}

void	appendMisc(std::string& _res, size_t contentSize) {
	_res.append("Content-Type: text/html");
	_res.append(NEW_VALUE);
	_res.append("Content-length: " + std::to_string(contentSize));
	_res.append(END_HEADER);
}

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
void	checkRequest(int* status, std::string const & path)
{
	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) == 0)
	{
		if (S_ISREG(path_stat.st_mode))
			*status = OK;
		else
			*status = NOT_FOUND;
	}
	else
	{	
		std::cerr << COLOR_RED << "Error opening file: " << path << COLOR_RESET << "\n";
		*status = NOT_FOUND;
	}
}