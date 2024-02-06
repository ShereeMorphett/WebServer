#include "WebServerProg.hpp"
#include "../Color.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>

static std::string	absoluteToRelativePath(std::string root, std::string absoluePath)
{
	int rootLen = root.length();
	std::string relativePath = absoluePath.substr(rootLen, absoluePath.length());
	return(relativePath);
}

static std::string buildDirectoryLinks(DIR *directory, std::string path, clientData& client)
{
    std::string directoryFinding;
    struct dirent *en;
	std::string link;

    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + path + "</title>\n\
                        </head>\n\
                        <body>\n\
                            <h1>AUTOINDEX</h1>\n\
                            <p>\n";

    while ((en = readdir(directory)) != NULL)
    {
		if (!strncmp(en->d_name, ".", 1))
			continue;

		link.append(absoluteToRelativePath(client._root, path));
		link.append("/");
		link.append(en->d_name);
		
        std::string fullLink = "<a href='" + link + "'>" + en->d_name + "</a>";

        directoryFinding += "\t\t<p>";
		directoryFinding += fullLink;
        directoryFinding += "</p>\n";
		link.clear();
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
	std::string path = startingPath;


    client._response.append(HTTP_HEADER);
    client._response.append(NEW_VALUE);
   	client._response.append("Content-Type: text/html\r\n");
    client._response.append(toString(client._status) + "\r\n");
    std::string directoryFinding;

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

    directoryFinding += buildDirectoryLinks(directory, startingPath, client);

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