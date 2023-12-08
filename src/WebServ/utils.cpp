

#include "utils.hpp"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include "../Color.hpp"
#include "WebServerProg.hpp"
#include <sys/socket.h>//

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
      if (value != nullptr)
         std::cout << value;                                 
      else
         std::cout << "Environment variable does not exist.";
      std::cout << "</td></tr>\n";
   }
   
//    cout << "</table><\n";
//    cout << "</body>\n";
//    cout << "</html>\n";

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
      envArray[24] = nullptr;

	for (int i = 0; envArray[i] != nullptr; i++) 
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
	args[2] = nullptr;
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