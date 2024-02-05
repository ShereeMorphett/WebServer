
#include <iostream>
#include "utils.hpp"
#include "../Color.hpp"
#include <sys/stat.h>

#include <iostream>

void skipNonPrintable(std::istream& stream)
{
    while (stream)
    {
        char c = stream.peek();
        if (std::isprint(c))
            return ;
        stream.get();
    }
}

bool isValidDirectory(const std::string& path)
{
	struct stat fileInfo;
	std::cout << COLOR_GREEN << "path in validir:	" << path << COLOR_RESET << std::endl;
    if (stat(path.c_str(), &fileInfo) != 0)
        return false;

    return S_ISDIR(fileInfo.st_mode);
}

bool isValidFile(const std::string& path)
{
	std::ifstream file(path);

	if (file.good()) {
		return true;
	}

	return false;
}

void skipWhitespace(std::istream& stream)
{
    while (stream)
    {
        char c = stream.peek();
        switch (c)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '\f':
            case '\v':
                stream.get();
                break;
            default:
                return;
        }
    }
}

int	countDepth(std::string path)
{
	int	depth = 0;

	for (size_t i = 0; i < path.size(); i++) {
		if (path[i] == '/')
			depth++;
	}

	return depth;
}

void printLocation(location &location)
{
	std::cout << "\n_______________________________________" << std::endl;
	std::cout << "LocationPath:		"<< location.locationPath << std::endl;
	std::cout << "redirection:		"<< location.redirection << std::endl;
	std::cout << "root:		" << location.root << std::endl;
	std::cout << "listing:		" << location.listing << std::endl;
	std::cout << "defaultFile:		"<< location.defaultFile << std::endl;
	std::cout << "cgiPath:		"<< location.cgiPath << std::endl;
	std::cout << COLOR_CYAN << "Allowed methods (0 = POST, 1 =GET, 2 =  DELETE):"  << COLOR_RESET << std::endl;
	for (size_t i = 0; i < location.allowedMethods.size(); i++)
	{
		std::cout << location.allowedMethods[i] << "	";
	}
	std::cout << "\n_______________________________________" << std::endl;
}

void printServer(server &server)
{
        std::cout << server.serverName << ":    port: " << server.port << std::endl;
        std::cout << "client body max size: " << server.clientMaxBodySize << std::endl;
        std::cout << COLOR_CYAN << "Error pages map <number, page>" << COLOR_RESET << std::endl;
        printMap(server.errorPages);
        std::cout << COLOR_CYAN <<"printing server locations:	" << server.locations.size() << COLOR_RESET << std::endl; 
        for (size_t i = 0; i < server.locations.size(); i++)
			printLocation(server.locations[i]);
		std::cout << std::endl;
}

std::string parseStartingPath(std::string startingPath, std::string root)
{

    std::string parse = root;
	 std::cout << COLOR_GREEN << "Parse:	" << parse << COLOR_RESET << std::endl;
	size_t pos = startingPath.find(parse);
	if (pos != std::string::npos)
	{
		std::string resultPath = startingPath.substr(pos + parse.length());

		if (isValidDirectory(resultPath) && resultPath.back() != '/')
			resultPath.append("/");
		std::cout << COLOR_GREEN << "resultPath:	" << resultPath << COLOR_RESET << std::endl;
		return resultPath;
    } 
	else
	{
		return "";
    }
}