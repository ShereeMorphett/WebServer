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
		return resultPath;
    } 
	else
	{
		return "";
    }
}

static std::string buildLink(DIR *directory, std::string path, std::string referer)
{
    struct dirent *en;
    std::string directoryFinding;
	(void) referer;//
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
        std::string entryName = en->d_name;
        if (entryName != "." && entryName != ".." && entryName[0] != '.' && entryName != "obj" && entryName.substr(entryName.length() - 3) != ".md")
        {
			if (isDirectory(path) && path.back() != '/')
				path.append("/");
            std::string entryPath = path + entryName;
			std::cout << COLOR_GREEN << "Path		" << path << COLOR_RESET << std::endl;
            bool boolDirectory = (en->d_type == DT_DIR);
			// // std::string relativePath = entryPath.append("/");
			// std::cout << COLOR_GREEN << "relativePath		" << relativePath << std::endl;
            // size_t refererPos = referer.find_last_of('/');
            // std::string refererPath = referer.substr(0, refererPos + 1);
            std::string relativePath = parseStartingPath(entryPath);
			std::cout << COLOR_CYAN << "relativePath		" << relativePath << COLOR_RESET << std::endl;
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
    }
    directoryFinding += "\
                            </p>\n\
                        </body>\n\
                        </html>";

    return directoryFinding;
}


std::string WebServerProg::createDirectoryListing(std::string startingPath, std::string referer)
{
	(void) referer;//
    _response.append(HTTP_HEADER);
    _response.append(NEW_VALUE);
    _response.append("Content-Type: text/html\r\n");
    _response.append(toString(_status) + "\r\n");
    std::string directoryFinding;
    DIR *directory = opendir(startingPath.c_str());
	std::string basePath;
	if (startingPath == "/")
		basePath = "";
	// else
	// 	basePath = parseStartingPath(startingPath);
    if (directory == NULL)
    {
        std::cerr << COLOR_RED << "Error: could not open " << startingPath << COLOR_RESET << std::endl;
		_status = 404;
        return "";
    }
    directoryFinding += "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                            <title>" + basePath + "</title>\n\
                        </head>\n\
                        <body>\n\
                            <h1></h1>\n\
                            <p>\n";

    directoryFinding += buildLink(directory, startingPath, referer);

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