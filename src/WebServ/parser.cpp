
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "WebServerProg.hpp"

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

//std::vector<struct server>
void parseConfigFile(const char* fileName)
{
    std::ifstream file(fileName);
    std::string line;

    if (!file)
        throw std::runtime_error("File open error");

    std::vector<server> servers;

    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string key;
        std::string value;

        ss >> key >> value;
        if (key != "{" && key != "}")
            std::cout << key << value << std::endl;
        //populate data structures
    }

    file.close();

    //return servers; // Return the parsed server data.
}




