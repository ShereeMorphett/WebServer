
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "WebServerProg.hpp"
#include "../Color.hpp"

static int validateLocation(const server &servers)
{
    for (size_t i = 0; i < servers.locations.size() ; i++)
    {
        if (servers.locations[i].allowedMethods.size() == 0)
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        if (servers.locations[i].locationPath.empty())
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        // if (servers.locations[i].redirection.empty())
        //     throw std::runtime_error("Server config file is invalid: Invalid location in server"); can redirection be empty???
        if (servers.locations[i].listing < 0 || servers.locations[i].listing > 1)
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        if (servers.locations[i].root.empty())
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        if (servers.locations[i].defaultFile.empty())
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
    }
    return 0;
}


static int validateErrorPage(const server &servers)
{
    for (std::map<int, std::string>::const_iterator it = servers.errorPages.begin(); it != servers.errorPages.end(); it++)
    {
        if (it->first < 100 || it->first > 599)
            throw std::runtime_error("Server config file is invalid: Invalid Error Code in server");

        if (it->second.empty())
            throw std::runtime_error("Server config file is invalid: Empty Error Page Path in server");
    }
    return 0;
}

/*
    Port numbers 1024 - 65535 are available for the following user applications:
    Port numbers 1024 - 49151 are reserved for user server applications.
    Port numbers 49152 - 65535 are reserved for clients.
    If you have a server in the client port range (49152 - 65535), define that server in the TCP/IP network services database to prevent port number conflicts.
*/
void validateServers(const std::vector<struct server> &servers) //if there is any valid server it should run 
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        if(servers[i].port < 1024 && servers[i].port > 49151)
            throw std::runtime_error("Server config file is invalid: Invalid port");
        if(servers[i].clientMaxBodySize <= 0)
            throw std::runtime_error("Server config file is invalid: Client Max Body Size invalid");        
        if(servers[i].socketFD < 0)
            throw std::runtime_error("Server config file is invalid: Socket file descriptor invaild");        
        if (validateErrorPage(servers[i]))
         throw std::runtime_error("Server config file is invalid: Error pages invaild");
        if (validateLocation(servers[i]))
         throw std::runtime_error("Server config file is invalid: Locations invaild");
    }
    std::cout << COLOR_GREEN << "Servers are valid" << COLOR_RESET << std::endl;

}