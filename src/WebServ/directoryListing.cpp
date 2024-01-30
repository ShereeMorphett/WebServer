#include "WebServerProg.hpp"
#include "../Color.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>

static std::string parseStartingPath(std::string startingPath)
{

    std::string parse = "WebServer";

	size_t pos = startingPath.find(parse);
	if (pos != std::string::npos)
	{
		std::string resultPath = startingPath.substr(pos + parse.length());
		if (isDirectory(resultPath) && resultPath.back() != '/')
			resultPath.append("/");
		return resultPath;
    } 
	else
	{
		return "";
    }
}

static std::string buildDirectoryLinks(DIR *directory, std::string rootPath)
{
    struct dirent *en;
    std::string directoryFinding;

    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + rootPath + "</title>\n\
                        </head>\n\
                        <body>\n\
                            <h1>AUTOINDEX</h1>\n\
                            <p>\n";

    if (rootPath.back() != '/')
        rootPath.append("/");
    while ((en = readdir(directory)) != NULL)
    {
        std::string entryName = en->d_name;
        if (entryName == "." || entryName == ".." || entryName[0] == '.' || entryName == "obj")
            continue;
        std::string entryPath = rootPath + entryName;
        bool boolDirectory = (en->d_type == DT_DIR);
       if (boolDirectory && !isDirectory(entryPath))
        {
            std::cerr << "Error: " << entryPath << " is not a directory as expected." << std::endl;
            continue;
        }
        std::string relativePath = parseStartingPath(entryPath);
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


std::string WebServerProg::createDirectoryListing(std::string startingPath)
{
    _response.append(HTTP_HEADER);
    _response.append(NEW_VALUE);
    _response.append("Content-Type: text/html\r\n");
    _response.append(toString(_status) + "\r\n");
    std::string directoryFinding;
    DIR *directory = opendir(startingPath.c_str());

    std::cout << COLOR_MAGENTA << startingPath << COLOR_RESET << std::endl;
    if (directory == NULL)
    {
        std::cerr << COLOR_RED << "Error: could not open " << startingPath << COLOR_RESET << std::endl;
		_status = 404;
        return "";
    }
    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + startingPath + "</title>\n\
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
    _response.append("Content-Length:" + std::to_string(directoryFinding.length()));
    _response.append(END_HEADER);
    _response.append(directoryFinding);

    return directoryFinding;
}