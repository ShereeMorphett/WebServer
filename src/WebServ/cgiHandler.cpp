

#include "utils.hpp"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include "../Color.hpp"
#include "WebServerProg.hpp"
#include <sys/socket.h>//
#include <cstdlib> //
#include <sys/wait.h>//
#include <cstring>//
#include <cerrno>
#include <cstdio>


/*
These strings represent environment variables that are commonly used in the context of CGI (Common Gateway Interface) when building web servers.
    COMSPEC: The command processor for the Windows operating system.
    DOCUMENT_ROOT: The root directory of the server's documents.
    GATEWAY_INTERFACE: The CGI specification version supported by the server.
    HTTP_ACCEPT: The MIME types accepted by the browser.
    HTTP_ACCEPT_ENCODING: The content encoding accepted by the browser.
    HTTP_ACCEPT_LANGUAGE: The natural languages accepted by the browser.
    HTTP_CONNECTION: The type of connection established by the browser.
    HTTP_HOST: The hostname specified by the client in the HTTP request.
    HTTP_USER_AGENT: Information about the user agent (browser or client).
    PATH: The system's executable search path.
    QUERY_STRING: The query string part of the URL.
    REMOTE_ADDR: The IP address of the client making the request.
    REMOTE_PORT: The port number on the client machine being used for the communication.
    REQUEST_METHOD: The HTTP request method (GET, POST, etc.).
    REQUEST_URI: The resource requested in the HTTP request.
    SCRIPT_FILENAME: The full path to the script being executed.
    SCRIPT_NAME: The virtual path of the script.
    SERVER_ADDR: The server's IP address.
    SERVER_ADMIN: The email address for server administrator.
    SERVER_NAME: The server's hostname or IP address.
    SERVER_PORT: The port on which the server is listening.
    SERVER_PROTOCOL: The version of the protocol the client used (e.g., "HTTP/1.1").
    SERVER_SIGNATURE: A string that may be added to the response.
    SERVER_SOFTWARE: The name and version of the server software.
	
	should I set the default ones in Hardcode? what are the default settings we wish to use with CGI?
	*/

const char* serverEnviroment[24] =
{
   "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",   
   "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
   "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",         
   "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
   "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",      
   "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
   "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",      
   "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
   "SERVER_SIGNATURE","SERVER_SOFTWARE" 
};


std::string runCgi() //may end up being a class?
{
    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << "<html>\n";
    std::cout << "<head>\n";
    std::cout << "<title>CGI Environment Variables</title>\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";
    std::cout << "<table border = \"0\" cellspacing = \"2\">";

   for (int i = 0; i < 24; i++)
   {
      std::cout << "<tr><td>" << serverEnviroment[i] << "</td><td>"; 
    
      char *value = getenv(serverEnviroment[i]);  
      if (value != NULL)
         std::cout << value;                                 
      else
         std::cout << "Environment variable does not exist.";
      std::cout << "</td></tr>\n";
   }
   
   std::cout << "</table><\n";
   std::cout << "</body>\n";
   std::cout << "</html>\n";

   int pipesIn[2];
   int pipesOut[2];
   pipe(pipesIn);
   pipe(pipesOut);

   int cgiPid;

   if ((cgiPid = fork()) == 0)
   {
      char* envArray[25];
      for (int i = 0; i < 24; i++)
	  {
         envArray[i] = getenv(serverEnviroment[i]);
      }
      envArray[24] = NULL;

	for (int i = 0; envArray[i] != NULL; i++) 
	{
		std::cout << serverEnviroment[i] << "=" << envArray[i] << std::endl;
	}
	dup2(pipesIn[0], STDIN_FILENO);
	dup2(pipesOut[1], STDOUT_FILENO);
	close(pipesOut[0]);
	close(pipesIn[1]);

	const char* dataToSend = "first_name=John&last_name=Doe";
	write(STDIN_FILENO, dataToSend, strlen(dataToSend));

	// std::cout << COLOR_MAGENTA <<" hitting execve" << COLOR_RESET << std::endl;
	const char* args[3];

	args[0] = "/usr/local/bin/python3";
	args[1] = "/Users/smorphet/Desktop/WebServer/src/cgi-bin/hello_world.py"; //PLACE HOLDER FOR THE SERVER PATH
	args[2] = NULL;
	int result = execve(args[0], const_cast<char* const*>(args), envArray); //find the .cgi/ bin find first arg (where python/bash etc)
	if (result == -1)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}
	
   }
	//PARENT
	// Send data to the CGI script through the pipe

	//if sending from parent etc pipeIn[1]

	int status;
	waitpid(cgiPid, &status, 0);

	close(pipesOut[1]); // Close write end of the pipe
	char buffer[4096];
	bzero(buffer, sizeof(buffer));
	ssize_t bytesRead;
	// Read data from the CGI script through the pipe
	std::cout << COLOR_MAGENTA << "before sending" << std::endl;
	while ((bytesRead = read(pipesOut[0], buffer, sizeof(buffer))) > 0)
	{
		std::cout << COLOR_RED << buffer << std::endl;
		std::cout << COLOR_RED << bytesRead << std::endl;
	}

	
	std::cout << COLOR_MAGENTA << "after sending" << std::endl;
	std::cout << COLOR_MAGENTA << buffer << std::endl;
	std::cout << COLOR_MAGENTA << bytesRead << COLOR_RESET  << std::endl;
	close(pipesOut[1]);
	close(pipesOut[0]);
	close(pipesIn[0]);
	close(pipesIn[1]);
	return std::string(buffer, sizeof(buffer));

}