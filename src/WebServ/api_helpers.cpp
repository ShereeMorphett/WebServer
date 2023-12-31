#include "utils.hpp"
#include "api_helpers.hpp"
#include "Color.hpp"
#include <sstream>
#include <vector>
#include <unistd.h>


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

std::string	chooseErrorPage(int status) {
	switch (status) {
		case FORBIDDEN:
			return FORBIDDEN_PAGE;
		
		case UNAUTHORIZED:
			return UNAUTHORIZED_PAGE;

		case NOT_FOUND:
			return NOT_FOUND_PAGE;

		default:
			break; 
	}
	return SERVER_PAGE;
}

void	appendStatus(std::string& _res, int status) {
	_res.append(HTTP_HEADER);
	_res.append(toString(status));
	switch (status) {
		case OK:
			_res.append(" OK");
			break;
		
		case CREATED:
			_res.append(" Created");
			break;

		case NOT_FOUND:
			_res.append(" Not Found");
			break;

		case UNAUTHORIZED:
			_res.append(" Unauthorized");
			break;
		
		case FORBIDDEN:
			_res.append(" Forbidden");
			break;
		default:
			break;
	}
	_res.append(NEW_VALUE);
}