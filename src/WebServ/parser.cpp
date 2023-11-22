
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

enum Methods
{
    POST,
    GET,
    DELETE
};

enum Methods
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

struct server
{
    int client_max_body_size;
    std::string serverName;
    std::map<int, std::string> errorPages; //this might be over kill
    std::vector<location> locations; 
};

struct location
{
    std::vector<int> allowedMethods;
    std::string locationPath;
    std::string redirection;
    std::string root;
    int listing;
    std::string defaultFile;
    std::string cgiPath;
};


std::vector<struct server> parseConfigFile(std::string fileName)
{
    std::ifstream configFile;
    std::stringstream ss;

    // if (open(fileName))
    //     return error;



}