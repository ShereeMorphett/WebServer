#include "api_helpers.hpp"

std::string extractPath(const std::string& request)
{
    size_t firstSpace = request.find(' ');
    if (firstSpace == std::string::npos)
	{
        return ""; // Invalid request format
    }

    size_t secondSpace = request.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos)
	{
        return ""; // Invalid request format
    }
    return request.substr(firstSpace + 1, secondSpace - firstSpace - 1);
}

std::string getFileExtension(const std::string& fileName) {
    // Find the last dot character in the filename
    size_t dotPosition = fileName.find_last_of(".");

    if (dotPosition != std::string::npos && dotPosition != fileName.length() - 1) {
        // Extract and return the substring after the last dot
        return fileName.substr(dotPosition + 1);
    } else {
        // No extension found or the dot is the last character
		std::cout << "Invalid file, no extension \n";
        return "";
	}
}

std::string	readFile(const std::string& path, int* status) {
	std::ifstream	inFile;
	std::string		line;
	std::string		body;

	inFile.open("." + path);
	if (!inFile) {
		if (*status == 404) {
			return body;
		}
		*status = NOT_FOUND;
		body = readFile("/src/defaults/404.html", status);
	} else {
		while (std::getline(inFile, line)) {
			body.append(line);
		}
		inFile.close();
	}
	
	return body;
}