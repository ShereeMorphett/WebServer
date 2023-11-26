
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "WebServerProg.hpp"
#include "../Color.hpp"
#include <cstdlib>


enum Listing
{
    ON,
    OFF
};

/*
server {
   listen `host:8080`;
         server_name  `server_name`;  <- optional
   error_page  404  /404.html;
   error_page   500 502 503 504  /50x.html;
   client_max_body_size 255;

   location / {
      allow GET;
      allow POST;
      redirection /new_path;
      root   /usr/share/nginx/html;
      listing on/off;
      default_file /index.html;
      cgi_path /folder/file.cgi
      CGI stuff?
   }
   location /example_folder {
      allow POST;
      redirection /new_example_folder;
      root   /usr/share/nginx/html;
      listing on/off;
      default_file /index.html;
      CGI stuff?
   }
}
*/
// struct location
// {
//     std::vector<int> allowedMethods;
//     std::string locationPath;
//     std::string redirection;
//     std::string root;
//     int listing;
//     std::string defaultFile;
//     std::string cgiPath;
// };

// struct server
// {
//		int port;
//		std::string serverName;
//     std::map<int, std::string> errorPages; //this might be over kill
//     std::vector<location> locations; 
//     int clientMaxBodySize;
// };

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
template <typename Type,typename T>
void printMap(typename std::map<Type, T> theMap)
{
	for (typename std::map<Type, T>::iterator it = theMap.begin(); it != theMap.end(); ++it)
	{
        std::cout << it->first << ": " << it->second << std::endl;
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

location parseLocation(std::istream &stream, std::string extValue)
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
			temp.root = value;
		else if (key == "listing")
		{
			temp.listing = ON; //setting as on by default until i know what it does :D
			if (value == "off")
				temp.listing = OFF;	
		}
		else if (key == "default_file")
			temp.defaultFile = value;
		else if (key == "cgi_Path")
			temp.cgiPath = "PLACEHOLDER TEXT";
		line.clear();
    }
	return temp;
}

std::map<int, std::string> parseErrorFile(const std::string &line)
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

server parseServer(std::istream &stream)
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
		else
		{
			std::cout << COLOR_CYAN << line << COLOR_RESET << std::endl;
			std::cout << COLOR_GREEN << key << "	 		" << value << COLOR_RESET << std::endl;
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
		//std::cout << COLOR_RED << line << COLOR_RESET << std::endl;
        std::stringstream sstream(line);
        sstream >> key;
		// std::cout << COLOR_GREEN << key << COLOR_RESET << std::endl;
        if (key == "server")
        {
            try
            {
                servers.push_back(parseServer(stream));
            }
            catch (const std::exception& e)
            {
                std::cerr << COLOR_RED << e.what() << COLOR_RESET << std::endl;
                exit(EXIT_FAILURE); //might want a better way to do this
            }
        }
        line.clear();
    }
	for (size_t i = 0; i < servers.size(); i++)
		printServer(servers[i]);
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




