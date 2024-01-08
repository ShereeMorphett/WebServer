
#include <string>
#include <map>

class CgiHandler
{
    private:
        void setupEnvironment(std::string _request, char method, const std::string& scriptPath);
        void executeCgi(const std::string& scriptName);
        std::string readCgiOutput(int pipesOut[2]);
        std::map<std::string, std::string> cgiEnvironment;
    
    public:
        std::string runCgi(const std::string& scriptPath, std::string & _request, char method);
        CgiHandler(); //probably dont need defaults
        ~CgiHandler();
};