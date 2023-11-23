
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "WebServerProg.hpp"
#include "../Color.hpp"
#include <cstdlib>

enum Methods
{
    POST,
    GET,
    DELETE
};

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
// 	int port;
// 	std::string serverName;
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


void printServer(server &server)
{
        std::cout << server.serverName << ":    port: " << server.port << std::endl;
        std::cout << "client body max size: " << server.clientMaxBodySize << std::endl;
        std::cout << "Error pages map <number, page>" << std::endl;
        //printErrorMap();
        std::cout << "printing server locations:"<< std::endl; 
        //printServerLocations();
}

location parseLocation(std::istream &stream)
{
    char c;
     while (stream.get(c) && c != '}')
    {
        // skipNonPrintable(stream);
        // line += c;
        std::cout << c ;
    }
    location temp;
    temp.defaultFile = "testing";
    ( void) stream;
    return temp;
}

std::map<int, std::string> parseErrorFile(std::string &line)
{
    int key;
    std::string value;
    std::map<int, std::string> tempMap;
    std::stringstream sstream(line);
    std::cout << line << std::endl;
    sstream >> key >> value;
    tempMap[key] = value;

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
        while (stream.get(c) && c != ';')
        {
            skipNonPrintable(stream);
            line += c;
        }
        std::stringstream sstream(line);
        sstream >> key >> value;
        if (key == "error_page") //need to split value into maps
        {
            std::cout << COLOR_CYAN << key << COLOR_RESET << std::endl;
            std::cout << COLOR_MAGENTA << value << COLOR_RESET << std::endl;
            if (temp.errorPages.size() == 0)
                temp.errorPages = parseErrorFile(line);
            else
            {
                std::map<int, std::string> tempMap = parseErrorFile(line);
                temp.errorPages .insert(tempMap.begin(), tempMap.end());
            }
        }
        else if (key == "server_name")
            temp.serverName = value;
        else if (key == "location")
        {
            parseLocation(stream);
            // temp.locations.push_back(parseLocation(stream));
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
            int size;
            std::stringstream sizeStream(value);
            sizeStream >> size;
            temp.clientMaxBodySize = size;
        }
        std::cout << key << std::endl;
        line.clear();
    }
    printServer(temp);
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
        while (stream.get(c) && c != '{')
            line += c;
        std::stringstream sstream(line);
        std::cout << line << std::cout;
        sstream >> key;
        if (key == "server")
        {
            std::cout << COLOR_YELLOW << key << "|"<< COLOR_RESET << std::endl;
            try
            {
                servers.push_back(parseServer(stream));
            }
            catch (const std::exception& e)
            {
                std::cerr << COLOR_RED << e.what() << COLOR_RESET << std::endl;
                exit(EXIT_FAILURE); //might want a better way to do this?
            }
        }
        line.clear(); //
    }

    return servers;
}

//std::vector<struct server>
void parseConfigFile(const std::string& fileName)
{
    std::vector<struct server> servers;

    std::ifstream file(fileName.c_str());
    if (!file)
        throw std::runtime_error("File open error");
    
    servers = parseConfigFile(file);
    //return servers;
}




