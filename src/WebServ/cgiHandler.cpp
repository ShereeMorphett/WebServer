
// #include <iostream>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <string>
// #include "../Color.hpp"
// #include "WebServerProg.hpp"
// #include <sys/socket.h>//

// const char* serverEnviroment[24] =
// {
//    "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",   
//    "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
//    "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",         
//    "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
//    "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",      
//    "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
//    "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",      
//    "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
//    "SERVER_SIGNATURE","SERVER_SOFTWARE" 
// };


// void runCgi(int clientSocket) //may end up being a class?
// {
// //    cout << "Content-type:text/html\r\n\r\n";
// //    cout << "<html>\n";
// //    cout << "<head>\n";
// //    cout << "<title>CGI Environment Variables</title>\n";
// //    cout << "</head>\n";
// //    cout << "<body>\n";
// //    cout << "<table border = \"0\" cellspacing = \"2\">";

//    for (int i = 0; i < 24; i++)
//    {
//       std::cout << "<tr><td>" << serverEnviroment[i] << "</td><td>"; 
    
//       char *value = getenv(serverEnviroment[i]);  
//       if (value != nullptr)
//          std::cout << value;                                 
//       else
//          std::cout << "Environment variable does not exist.";
//       std::cout << "</td></tr>\n";
//    }
   
// //    cout << "</table><\n";
// //    cout << "</body>\n";
// //    cout << "</html>\n";

//    int pipes[2];
//    pipe(pipes);

//    int cgiPid;

//    if ((cgiPid = fork()) == 0)
//    {
//       // Set up environment variables as char* array
//       char* envArray[25];
//       for (int i = 0; i < 24; i++)
// 	  {
//          envArray[i] = getenv(serverEnviroment[i]);
//       }
//       envArray[24] = nullptr;

// 	for (int i = 0; envArray[i] != nullptr; i++) 
// 	{
// 		std::cout << serverEnviroment[i] << "=" << envArray[i] << std::endl;
// 	}
// 	  dup2(pipes[0], STDIN_FILENO);
//       close(pipes[1]);
// 	  close(pipes[0]);


// 	std::cout << COLOR_MAGENTA <<" hitting execve" << COLOR_RESET << std::endl;
// 	const char* args[3];

// 	args[0] = "/usr/local/bin/python3";
// 	args[1] = "/Users/smorphet/Desktop/WebServer/src/cgi-bin/hello_world.py";
// 	args[2] = nullptr;

// 	int result = execve(args[0], const_cast<char* const*>(args), envArray); //find the .cgi/ bin find first arg (where python/bash etc)
// 	if (result == -1)
// 	{
// 		perror("execve");
// 		exit(EXIT_FAILURE);
// 	}
//    }

// 	// close(pipes[0]); 
// 	// Send data to the CGI script through the pipe

// 	char dataToSend[4096];
// 	write(pipes[1], dataToSend, strlen(dataToSend));

// 	// Close writing end of the pipe
// 	close(pipes[1]);

// 	// Wait for the CGI process to finish (optional)
// 	int status;
// 	waitpid(cgiPid, &status, 0);

// 	// Read and send CGI script output
// 	char buffer[4096];
// 	ssize_t bytesRead;
// 	while ((bytesRead = read(pipes[0], buffer, sizeof(buffer))) > 0)
// 	{
// 		send(clientSocket, buffer, bytesRead, 0);
// 	}
// 	int i = 0;
// 	while (i < 30) 
// 	{
// 		std::cout << COLOR_MAGENTA << buffer[i] << std::endl;
// 		i++;
// 	}
// 	std::cout << COLOR_MAGENTA << buffer << std::endl;
// 	std::cout << COLOR_MAGENTA << sizeof(buffer) << std::endl;
// 	std::cout << COLOR_MAGENTA << bytesRead << std::endl;
// 	close(pipes[0]);
// }