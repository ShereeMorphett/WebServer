#include "WebServerProg.hpp"
#include "../Color.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>

static std::string buildDirectoryLinks(DIR *directory, std::string path)
{
    struct dirent *en;
    std::string directoryFinding;

    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + path + "</title>\n\
                        </head>\n\
                        <body>\n\
                            <h1>AUTOINDEX</h1>\n\
                            <p>\n";

    if (path.back() != '/')
        path.append("/");
    while ((en = readdir(directory)) != NULL)
    {
        std::string entryName = en->d_name;
        if (entryName == "." || entryName == ".." || entryName[0] == '.' || entryName == "obj")
            continue;
        std::string entryPath = "." + path + entryName;
		std::cout << COLOR_GREEN << "entry		" << entryPath << COLOR_RESET << std::endl;
        bool boolDirectory = (en->d_type == DT_DIR);
		std::cout << COLOR_GREEN << boolDirectory << COLOR_RESET << std::endl;
       if (boolDirectory && !isValidDirectory(entryPath))
        {
            std::cerr << "Error: " << entryPath << " is not a directory as expected." << std::endl;
            continue;
        }

		std::cout << "build directoery links: " << entryPath << std::endl;

        // std::string relativePath = parseStartingPath(entryPath, path);
		std::string relativePath = entryPath;
		std::cout << COLOR_MAGENTA << relativePath << COLOR_RESET << std::endl;
        std::string fullLink = "<a href='" + relativePath + "'>" + entryName + "</a>";
        directoryFinding += "\t\t<p>";
        if (boolDirectory)
        {
            directoryFinding += fullLink + "\n";
        }
        else
        {
            directoryFinding += fullLink;
        }

        directoryFinding += "</p>\n";
    }
    directoryFinding += "\
                            </p>\n\
                        </body>\n\
                        </html>";

    return directoryFinding;
}


std::string WebServerProg::createDirectoryListing(int clientSocket, std::string startingPath)
{
	clientData&	client = accessClientData(clientSocket);
	std::string path = "." + startingPath;


    client._response.append(HTTP_HEADER);
    client._response.append(NEW_VALUE);
   	client._response.append("Content-Type: text/html\r\n");
    client._response.append(toString(client._status) + "\r\n");
    std::string directoryFinding;
	std::cerr << COLOR_GREEN << "start " << startingPath << COLOR_RESET << std::endl;
    std::cerr << COLOR_GREEN << "Dir " << path << COLOR_RESET << std::endl;
    DIR *directory = opendir(path.c_str());
    if (directory == NULL)
    {
        std::cerr << COLOR_RED << "Error: could not open " << path << COLOR_RESET << std::endl;
		client._status = NOT_FOUND;
        return "";
    }
    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + path + "</title>\n\
                        </head>\n\
                        <body>\n\
                            <h1></h1>\n\
                            <p>\n";

    directoryFinding += buildDirectoryLinks(directory, startingPath);

    directoryFinding += "\
                            </p>\n\
                        </body>\n\
                        </html>";
    closedir(directory);
    client._response.append("Content-Length:" + std::to_string(directoryFinding.length()));
    client._response.append(END_HEADER);
    client._response.append(directoryFinding);

    return directoryFinding;
}