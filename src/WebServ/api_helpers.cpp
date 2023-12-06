#include "api_helpers.hpp"

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
std::string	readFile(std::string const & path, int* status) {
	std::ifstream	inFile;
	std::string		line;
	std::string		body;

	inFile.open("." + path);
	if (!inFile) {
		if (*status == 404) {
			return body;
		}
		*status = NOT_FOUND;
		body = readFile(ERROR_PAGE, status);
	} else {
		while (std::getline(inFile, line)) {
			body.append(line);
		}
		inFile.close();
	}
	
	return body;
}