
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "WebServerProg.hpp"
#include "../Color.hpp"
#include <cstdlib>
#include "utils.hpp"
#include "constants.hpp"
#include <unistd.h>
#include <float.h>

static	std::string	checkRoot(std::string& root)
{
	
    if (root[0] != '/')
		throw std::runtime_error("Root path must start with '/'");
	else  if (root[root.size() - 1] == '/' && root.size() != 1)
		throw std::runtime_error("Root path must NOT end with '/'");
	else if (root.find('.') != std::string::npos)
		throw std::runtime_error("Root path must NOT contain '.'");
	
	return root;
}

static std::string	checkDefaultFile(std::string& defaultFile)
{
	if (defaultFile.find('/') != std::string::npos)
		throw std::runtime_error("Default file path must NOT contain '/', include only file name.");
	if (defaultFile.find('.') == std::string::npos)
		throw std::runtime_error("Default file path must contain '.', please enter valid filename.");

	return defaultFile;
}

static Location parseLocation(std::istream &stream, std::string extValue)
{
    Location temp;
    char c;

    temp.locationPath = extValue;
	temp.redirection = false;
	temp.listing = false;

	std::string line;
	while (stream.get(c))
	{
        skipNonPrintable(stream);
		while (stream.get(c) && c != ';')
        {
			line += c;
			char check = stream.peek();
			if (check == '}')
				return temp;
		}
		std::stringstream sstream(line);
		std::string key;
		std::string value;
		sstream >> key >> value;
		if (key == "allow")
			temp.allowedMethods.push_back(value);
		else if (key == "return") {
			temp.redirection = true;
			temp.redirStatus = std::stoi(value);
			sstream >> value;
			temp.redirLocation = value;
		}
		else if (key == "root")
			temp.root = checkRoot(value);
		else if (key == "listing")
		{
			if (value == "on")
				temp.listing = true;
		}
		else if (key == "default_file")
			temp.defaultFile = checkDefaultFile(value);
		else if (key == "cgi_path")
        	temp.cgiPath = value; 
        else if (key == "alias")
            temp.alias = value;
		line.clear();
    }
	return temp;
}

static std::map<int, std::string> parseErrorFile(const std::string &line)
{
    std::map<int, std::string> tempMap;
    std::stringstream sstream(line);
    std::string directive;
	int key;
    sstream >> directive;

    if (directive != "error_page")
	{
        std::cerr << COLOR_RED << "Invalid directive: " << directive << COLOR_RESET << std::endl;
        return tempMap;
    }
    while (sstream >> key)
	{
	    std::stringstream keystream;
	    keystream << key;
		tempMap[key] = "/" + keystream.str() + ".html";
    }
    return tempMap;
}

static Server parseServer(std::istream &stream)
{
    Server temp;
    std::string line;
    while (stream)
    {
        char c;
        std::string key;
        std::string value;
        skipWhitespace(stream);
        while (stream.get(c) && c != ';' && c != '{')
        {
            skipNonPrintable(stream);
            if (c == '}')
				return temp;
			else
				line += c;
        }
        std::stringstream sstream(line);
        sstream >> key >> value;
        if (key == "error_page")
        {
            if (temp.errorPages.size() == 0)
                temp.errorPages = parseErrorFile(line);
            else
            {
                std::map<int, std::string> tempMap = parseErrorFile(line);
                temp.errorPages.insert(tempMap.begin(), tempMap.end());
            }
        }
        else if (key == "server_name")
		{
            temp.serverName = value;
		}
        else if (key == "location")
		{
			temp.locations.push_back(parseLocation(stream, value));
		}
        else if (key == "listen")
        {
            int port;
            std::stringstream portStream(value);
            while (portStream.get(c) && c != ':')
            {
                continue;
            }
            portStream >> port;
            temp.port = port;
        }
        else if (key == "client_max_body_size")
        {
            double size;
            std::stringstream sizeStream(value);
            sizeStream >> size;
            temp.clientMaxBodySize = size;
            if (size == DBL_MAX)
                std::cout << COLOR_YELLOW << "WARNING: clientMaxBody exceeds double. Client max body size set to 1.7976931348623158e+308" << COLOR_RESET << std::endl;
        }
		else if (key == "uploadDirectory")
        {
            temp.uploadDirectory = value;
			createDirectory(temp.uploadDirectory);
        }
        line.clear();
    }
	std::cout << COLOR_GREEN << "RETURNING TEMP: " << temp.serverName << COLOR_RESET << std::endl;
    return temp;
}

std::vector<struct Server> parseConfigFile(std::istream &stream)
{
    std::vector<Server> servers;
    std::string line;
    while (stream)
    {      
        char c;
        std::string key;
        std::string value;
        
        skipNonPrintable(stream);
        skipWhitespace(stream);
        while (stream.get(c) && c != '{' && c != '}')
            line += c;
        std::stringstream sstream(line);
        sstream >> key;
        if (key == "server")
        {
            try
            {
				Server temp = parseServer(stream);
                servers.push_back(temp);
            }
            catch (const std::exception& e)
            {
                std::cerr << COLOR_RED << e.what() << COLOR_RESET << std::endl;
            }
        }
        line.clear();
    }

    return servers;
}

std::vector<struct Server> parseConfigFile(const std::string& fileName)
{
    std::vector<struct Server> servers;

    std::ifstream file(fileName.c_str());
    if (!file)
        throw std::runtime_error("File open error");
    servers = parseConfigFile(file);
    return servers;
}