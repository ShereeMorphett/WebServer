
#include <string>
#include <map>

class CgiHandler
{
    private:
        void setupEnvironment();
        void executeCgi(const std::string& scriptName);
        std::string readCgiOutput(int pipesOut[2]);
        std::map<std::string, std::string> cgiEnvironment;

    
    public:
        CgiHandler(); //probably dont need defaults
        ~CgiHandler();

        std::string runCgi(const std::string& scriptPath);
};