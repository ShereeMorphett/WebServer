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

const char* serverEnviroment[22] =
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
    for (int i = 0; i < 22; ++i)
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

std::string extractShebang(const std::string& scriptName)
{
    std::ifstream scriptFile(scriptName);
    if (!scriptFile.is_open())
    {
        std::cerr << "Failed to open script file: " << scriptName << std::endl;
        return "";
    }

    std::string shebang;
    if (std::getline(scriptFile, shebang))
    {
        scriptFile.close();
        if (shebang.compare(0, 2, "#!") == 0)
        {
            std::istringstream lineStream(shebang.substr(2));
            std::string token;
            if (lineStream >> token)
            {
                return token;
            }
        }
    }

    return ""; 
}

void CgiHandler::executeCgi(const std::string& scriptName)
{
    const char* envArray[cgiEnvironment.size() + 1];
    int i = 0;
    for (const auto& entry : cgiEnvironment)
    {
        envArray[i++] = strdup((entry.first + "=" + entry.second).c_str());
    }
    envArray[i] = nullptr;
    const char* scriptArray[3];
	std::string interpreterPath = extractShebang(scriptName);

    scriptArray[0] = interpreterPath.c_str();
	scriptArray[1] = scriptName.c_str();
    scriptArray[2] = nullptr;

	std::cerr << "script arr 0: " << interpreterPath.c_str() << std::endl;
	std::cerr << "script arr 1: " << scriptName.c_str() << std::endl;

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
		std::cerr << "EXECUTING CGI" << std::endl;
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
    return cgiOutput;
}

CgiHandler::CgiHandler(std::multimap<std::string, std::string> requestData): _requestData(requestData)
{}

CgiHandler::~CgiHandler(){}