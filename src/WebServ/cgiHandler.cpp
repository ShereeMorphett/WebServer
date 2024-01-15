#include "utils.hpp"
#include <cstdio>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <cctype>
#include "../Color.hpp"
#include "CgiHandler.hpp"
#include "constants.hpp"

// These strings represent environment variables that are commonly used in the context of CGI (Common Gateway Interface) when building web servers.
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
//     SERVER_NAME: The server's hostname or IP address.
//     SERVER_PORT: The port on which the server is listening.
//     SERVER_PROTOCOL: The version of the protocol the client used (e.g., "HTTP/1.1").
//     SERVER_SIGNATURE: A string that may be added to the response.
//     SERVER_SOFTWARE: The name and version of the server software.
	
// 	should I set the default ones in Hardcode? what are the default settings we wish to use with CGI?
// 	*/


const char* serverEnviroment[24] =
{
   "DOCUMENT_ROOT", "GATEWAY_INTERFACE",   
   "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
   "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",         
   "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
   "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",      
   "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
   "SCRIPT_NAME", "SERVER_ADDR",    
   "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
   "SERVER_SIGNATURE","SERVER_SOFTWARE" 
};


// Key: Accept   Value: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Key: Accept-Encoding   Value: gzip, deflate, br
// Key: Accept-Language   Value: en-US,en;q=0.9
// Key: Body   Value: name=this&lastName=stuff&submit=Submit
// Key: Cache-Control   Value: max-age=0
// Key: Connection   Value: keep-alive
// Key: Content-Length   Value: 38
// Key: Content-Type   Value: application/x-www-form-urlencoded
// Key: HTTP-version   Value: HTTP/1.1
// Key: Host   Value: localhost:8080
// Key: Method   Value: POST
// Key: Origin   Value: http://localhost:8080
// Key: Path   Value: /Users/smorphet/Desktop/WebServer/src/cgi-bin/form.sh
// Key: Referer   Value: http://localhost:8080/test/form.html
// Key: Sec-Fetch-Dest   Value: document
// Key: Sec-Fetch-Mode   Value: navigate
// Key: Sec-Fetch-Site   Value: same-origin
// Key: Sec-Fetch-User   Value: ?1
// Key: Upgrade-Insecure-Requests   Value: 1
// Key: User-Agent   Value: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36
// Key: sec-ch-ua   Value: "Chromium";v="116", "Not)A;Brand";v="24", "Google Chrome";v="116"
// Key: sec-ch-ua-mobile   Value: ?0
// Key: sec-ch-ua-platform   Value: "macOS" 

static std::string getData(std::multimap<std::string, std::string>& _requestData, const std::string& key)
{
    auto range = _requestData.equal_range(key);
    for (auto it = range.first; it != range.second; ++it)
	{
        return it->second;
    }
    return "";
}


void CgiHandler::setupEnvironment(const std::string& scriptPath, int pipesIn[2], std::string& _request)
{
    for (int i = 0; i < 24; ++i)
    {
        char* value = getenv(serverEnviroment[i]);
        if (value != NULL)
            cgiEnvironment[serverEnviroment[i]] = value;
    }
	cgiEnvironment["GATEWAY_INTERFACE"] = std::string("CGI/1.1");
    cgiEnvironment["SCRIPT_FILENAME"] = scriptPath;
    cgiEnvironment["PATH_INFO"] = scriptPath;
    cgiEnvironment["PATH_TRANSLATED"] = scriptPath;
	std::string port = getData(_requestData, "Host");
	if (port.length() >= 4)
        port = port.substr(port.length() - 4);
    cgiEnvironment["SERVER_PORT"] = port;
    cgiEnvironment["REQUEST_METHOD"] = getData(_requestData, "Method");
    cgiEnvironment["SERVER_PROTOCOL"] = getData(_requestData, "HTTP-version");
	cgiEnvironment["QUERY_STRING"] = getData(_requestData, "Body");
    cgiEnvironment["REQUEST_URI"] = scriptPath + "?" + cgiEnvironment["QUERY_STRING"];
	dup2(pipesIn[0], STDIN_FILENO);
	close(pipesIn[1]);
	write(pipesIn[0], _request.c_str(), _request.length());
	close(pipesIn[0]);
}

void CgiHandler::executeCgi(const std::string& scriptName)
{
    const char* envArray[cgiEnvironment.size() + 1];
    int i = 0;
    std::cerr << COLOR_MAGENTA << "PRINTING ENV ARRAY \n\n"<< COLOR_RESET << std::endl;
    for (const auto& entry : cgiEnvironment)
    {
        envArray[i++] = strdup((entry.first + "=" + entry.second).c_str());
        std::cerr << COLOR_MAGENTA << envArray[i - 1] << "\n" << COLOR_RESET << std::endl;
    }
    envArray[i] = nullptr;
    const char* scriptArray[3];
    scriptArray[0] = "/Users/smorphet/.brew/bin/python3";
    scriptArray[1] = "src/cgi-bin/form.py";
    scriptArray[2] = nullptr;
    (void)scriptName;
    if (execve(scriptArray[0], const_cast<char* const*>(scriptArray), const_cast<char* const*>(envArray)) == -1)
    {
        perror("execve");
        std::cerr << COLOR_RED << "execve" << COLOR_RESET << std::endl;
    }
}


std::string CgiHandler::readCgiOutput(int pipesOut[2])
{
    close(pipesOut[1]); 
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

std::string CgiHandler::runCgi(const std::string& scriptPath, std::string& _request)
{
    int pipesIn[2];
    int pipesOut[2];
	
		std::cerr << COLOR_RED << "In CGI hangler" << COLOR_RESET << std::endl;
	printMultimap(_requestData);
	std::cerr << COLOR_RESET;
    setupEnvironment(scriptPath, pipesIn, _request);

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
        close(pipesIn[1]);
        close(pipesOut[0]);
        executeCgi(scriptPath);
    }
    else
    {
        // Parent process
        int status;
        waitpid(cgiPid, &status, 0);
        cgiOutput = readCgiOutput(pipesOut);
		close(pipesIn[0]);
		close(pipesIn[1]);
		close(pipesOut[0]);
		close(pipesOut[1]);
    }
	dup2(resetStdin, STDIN_FILENO);
	dup2(resetStdout, STDOUT_FILENO);
	std::cerr << COLOR_RED << cgiOutput << COLOR_RESET << std::endl;
    return cgiOutput;
}

CgiHandler::CgiHandler(std::multimap<std::string, std::string> requestData): _requestData(requestData)
{}

CgiHandler::~CgiHandler(){}