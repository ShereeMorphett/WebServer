
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



static	std::string	checkRoot(std::string& root)
{
	if (root[0] != '/')
		throw std::runtime_error("Root path must start with '/'");
	if (root[root.size() - 1] == '/')
		throw std::runtime_error("Root path must NOT end with '/'");
	if (root.find('.') != std::string::npos)
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

static location parseLocation(std::istream &stream, std::string extValue)
{
    char c;
    location temp;
    temp.locationPath = extValue;
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
		else if (key == "redirection")
			temp.redirection = value;
		else if (key == "root")
			temp.root = checkRoot(value);
		else if (key == "listing")
		{
			temp.listing = ON;
			if (value == "off")
				temp.listing = OFF;	
		}
		else if (key == "default_file")
			temp.defaultFile = checkDefaultFile(value);
		else if (key == "cgi_path")
        	temp.cgiPath = value; 
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
        std::cerr << "Invalid directive: " << directive << std::endl;
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

static server parseServer(std::istream &stream)
{
    server temp;
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
			char peek = stream.peek();
            if (c == '}' && peek == '}')
				return temp;
			if (c != '}')
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
            temp.serverName = value;
        else if (key == "location")
            temp.locations.push_back(parseLocation(stream, value));
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
            int size;
            std::stringstream sizeStream(value);
            sizeStream >> size;
            temp.clientMaxBodySize = size;
        }
        line.clear();
    }
    return temp;
}

std::vector<struct server> parseConfigFile(std::istream &stream)
{
    std::vector<server> servers;
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
                servers.push_back(parseServer(stream));
            }
            catch (const std::exception& e)
            {
                std::cerr << COLOR_RED << e.what() << COLOR_RESET << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        line.clear();
    }
    return servers;
}

std::vector<struct server> parseConfigFile(const std::string& fileName)
{
    std::vector<struct server> servers;

    std::ifstream file(fileName.c_str());
    if (!file)
        throw std::runtime_error("File open error");
    servers = parseConfigFile(file);
    return servers;
}




