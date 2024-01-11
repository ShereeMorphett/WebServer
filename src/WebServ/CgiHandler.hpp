
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <map>

class CgiHandler
{
    private:
        std::map<std::string, std::string> cgiEnvironment;
    public:
		void setupEnvironment(std::string _request, char method, const std::string& scriptPath, int pipesIn[2]);
		void executeCgi(const std::string& scriptName);
		std::string readCgiOutput(int pipesOut[2]);
		std::string runCgi(const std::string& scriptPath, std::string& _request, char method);
        CgiHandler();
        ~CgiHandler();
};

#endif 