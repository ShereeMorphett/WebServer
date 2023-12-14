
#include "utils.hpp"
#include <cstdio>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include "../Color.hpp"
#include "CgiHandler.hpp"

// These strings represent environment variables that are commonly used in the context of CGI (Common Gateway Interface) when building web servers.
//     COMSPEC: The command processor for the Windows operating system.
//     DOCUMENT_ROOT: The root directory of the server's documents.
//     GATEWAY_INTERFACE: The CGI specification version supported by the server.
//     HTTP_ACCEPT: The MIME types accepted by the browser.
//     HTTP_ACCEPT_ENCODING: The content encoding accepted by the browser.
//     HTTP_ACCEPT_LANGUAGE: The natural languages accepted by the browser.
//     HTTP_CONNECTION: The type of connection established by the browser.
//     HTTP_HOST: The hostname specified by the client in the HTTP request.
//     HTTP_USER_AGENT: Information about the user agent (browser or client).
//     PATH: The system's executable search path.
//     QUERY_STRING: The query string part of the URL.
//     REMOTE_ADDR: The IP address of the client making the request.
//     REMOTE_PORT: The port number on the client machine being used for the communication.
//     REQUEST_METHOD: The HTTP request method (GET, POST, etc.).
//     REQUEST_URI: The resource requested in the HTTP request.
//     SCRIPT_FILENAME: The full path to the script being executed.
//     SCRIPT_NAME: The virtual path of the script.
//     SERVER_ADDR: The server's IP address.
//     SERVER_ADMIN: The email address for server administrator.
//     SERVER_NAME: The server's hostname or IP address.
//     SERVER_PORT: The port on which the server is listening.
//     SERVER_PROTOCOL: The version of the protocol the client used (e.g., "HTTP/1.1").
//     SERVER_SIGNATURE: A string that may be added to the response.
//     SERVER_SOFTWARE: The name and version of the server software.
	
// 	should I set the default ones in Hardcode? what are the default settings we wish to use with CGI?
// 	*/


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



void CgiHandler::setupEnvironment()
{
    for (int i = 0; i < 24; ++i)
    {
        char* value = getenv(serverEnviroment[i]);
        if (value != NULL)
            cgiEnvironment[serverEnviroment[i]] = value;
    }
}
void CgiHandler::executeCgi(const std::string& scriptName)
{
    char* envArray[cgiEnvironment.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::const_iterator it = cgiEnvironment.begin(); it != cgiEnvironment.end(); ++it)
    {
        envArray[i++] = strdup((it->first + "=" + it->second).c_str());
    }
    envArray[i] = NULL;
    if (execve(scriptName.c_str(), const_cast<char* const*>(envArray), NULL) == -1)
    {
        std::cout << COLOR_RED << "|" << scriptName << "|" << COLOR_RESET << std::endl;
        perror("execve");
    }
    exit(EXIT_FAILURE);
}


std::string CgiHandler::readCgiOutput(int pipesOut[2])
{
    close(pipesOut[1]); // Close write end of the pipe
    char buffer[4096];
    ssize_t bytesRead;

    std::string output;

    while ((bytesRead = read(pipesOut[0], buffer, sizeof(buffer))) > 0)
    {
        output.append(buffer, bytesRead);
    }

    close(pipesOut[0]);

    return output;
}

std::string CgiHandler::runCgi(const std::string& scriptPath)
{
    setupEnvironment();

    int pipesIn[2];
    int pipesOut[2];

    pipe(pipesIn);
    pipe(pipesOut);

    int resetStdin = dup(STDIN_FILENO);
    int resetStdout = dup(STDOUT_FILENO);
    pid_t cgiPid;
    std::string cgiOutput;
    if ((cgiPid = fork()) == 0)
    {
        // Child process
        dup2(pipesIn[0], STDIN_FILENO);
        dup2(pipesOut[1], STDOUT_FILENO);
        close(pipesOut[0]);
        close(pipesIn[1]);

        executeCgi(scriptPath);
    }
    else
    {
        // Parent process
        int status;
        waitpid(cgiPid, &status, 0);
        cgiOutput = readCgiOutput(pipesOut);
    }
    close(pipesIn[0]);
    close(pipesIn[1]);
    close(pipesOut[1]);
    close(pipesOut[0]);
    dup2(resetStdin, STDIN_FILENO);
    dup2(resetStdout, STDOUT_FILENO);
    return cgiOutput; // should this be the buffer?
}



   CgiHandler::CgiHandler(){}

   CgiHandler::~CgiHandler(){}