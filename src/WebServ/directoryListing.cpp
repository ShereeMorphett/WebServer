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
	std::string webServer = "WebServer/";
	size_t pos = startingPath.find(webServer);
	if (pos != std::string::npos)
	{
		std::string resultPath = startingPath.substr(pos + webServer.length());
		if (isDirectory(resultPath) && resultPath.back() != '/')
			resultPath.append("/");
		std::cout << COLOR_GREEN << resultPath << COLOR_RESET << std::endl;
		return resultPath;
    } 
	else
	{
        std::cout << "WebServer not found in the path." << std::endl;
		return "";
    }
}

static std::string buildLink(DIR *directory, std::string path, int depth = 0)
{
    struct dirent *en;
    std::string directoryFinding;

    while ((en = readdir(directory)) != NULL)
    {
        std::string entryName = en->d_name;
        if (entryName != "." && entryName != ".." && entryName[0] != '.' && entryName != "obj")
        {
            std::string entryPath = path + entryName;
			if (isDirectory(entryPath) && entryPath.back() != '/')
				entryPath.append("/");
            bool isDirectory = (en->d_type == DT_DIR);
            std::string fullPath = entryPath;
            std::string relativePath = parseStartingPath(entryPath);
            std::string fullLink = "<a href='" + entryName + "'>" + entryName + "</a>";

            directoryFinding += "\t\t<p>";
			std::cout <<  COLOR_MAGENTA << entryPath << COLOR_RESET << std::endl;
            for (int i = 0; i < depth; ++i)
            {
                directoryFinding += "&emsp;";
            }

            if (isDirectory)
            {
                directoryFinding += fullLink + "\n";
                DIR *subDirectory = opendir(entryPath.c_str());
                if (subDirectory != NULL)
                {
                    directoryFinding += buildLink(subDirectory, entryPath, depth + 1);
                    closedir(subDirectory);
                }
            }
            else
                directoryFinding += fullLink;
            directoryFinding += "</p>\n";
        }
    }
    return directoryFinding;
}


std::string WebServerProg::createDirectoryListing(std::string startingPath)
{
    _response.append(HTTP_HEADER);
    _response.append(NEW_VALUE);
    _response.append("Content-Type: text/html\r\n");
    _response.append("OK\r\n");
    std::string directoryFinding;
    DIR *directory = opendir(startingPath.c_str());
	std::string basePath;
	if (startingPath == "/")
		basePath = "";
	else
		basePath = parseStartingPath(startingPath);
    if (directory == NULL)
    {
        std::cerr << COLOR_RED << "Error: could not open " << startingPath << COLOR_RESET << std::endl;
        return "";
    }
    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + basePath + "</title>\n\
                        </head>\n\
                        <body>\n\
                            <h1>AUTOINDEX</h1>\n\
                            <p>\n";

    directoryFinding += buildLink(directory, startingPath);

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