
#include <iostream>
#include "utils.hpp"
#include "../Color.hpp"
#include <sys/stat.h>

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

bool isDirectory(const std::string& path)
{
    struct stat fileInfo;
    
    if (stat(path.c_str(), &fileInfo) != 0)
        return false;
    return S_ISDIR(fileInfo.st_mode);
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
