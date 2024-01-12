
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <map>

class CgiHandler
{
    private:
        std::map<std::string, std::string> cgiEnvironment;
		std::multimap<std::string, std::string> _requestData;
    public:
		void setupEnvironment(const std::string& scriptPath, int pipesIn[2], std::string& _request);
		void executeCgi(const std::string& scriptName);
		std::string readCgiOutput(int pipesOut[2]);
		std::string runCgi(const std::string& scriptPath, std::string& _request);
        CgiHandler(std::multimap<std::string, std::string> requestData);
        ~CgiHandler();
};

#endif