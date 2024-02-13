
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "WebServerProg.hpp"
#include "../Color.hpp"
#include "utils.hpp"
#include <unistd.h>
#include "constants.hpp"

static bool	isValidRedirLocation(const Server &server, std::string const & newPath)
{
	for (size_t i = 0; i < server.locations.size(); i++) {
		if (newPath == server.locations[i].locationPath) {
			return true;
		}
	}

	return false;
}

static int validateLocation(const Server &server)
{
    for (size_t i = 0; i < server.locations.size() ; i++)
    {
        if (server.locations[i].allowedMethods.size() == 0)
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        if (server.locations[i].locationPath.empty())
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        if (server.locations[i].root.empty())
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
        if (server.locations[i].defaultFile.empty())
            throw std::runtime_error("Server config file is invalid: Invalid location in server");
		if (server.locations[i].redirection == true)
		{
			if (server.locations[i].redirStatus != TEMP_REDIR && server.locations[i].redirStatus != PERMA_REDIR)
				throw std::runtime_error("Server config file is invalid: Invalid location in server");
			if (!isValidRedirLocation(server, server.locations[i].redirLocation))
				throw std::runtime_error("Server config file is invalid: Invalid location in server");
		}
    }
    return 0;
}


static int validateErrorPage(const Server &servers)
{
    for (std::map<int, std::string>::const_iterator it = servers.errorPages.begin(); it != servers.errorPages.end(); it++)
    {
		char root[1024] = {};
		getcwd(root, sizeof(root));

        if (it->first < 100 || it->first > 599)
            throw std::runtime_error("Server config file is invalid: Invalid Error Code in server");
        if (it->second.empty())
            throw std::runtime_error("Server config file is invalid: Empty Error Page Path in server");

		std::string file;
		file.append(root);
		file.append("/defaults");
		file.append(it->second);
		if (!isValidFile(file))
			throw std::runtime_error("Error page defined but not present. Aborting server start! Add default files and try again :)");
			
    }
    return 0;
}

static int_least16_t validateServerConfigurations(const std::vector<struct Server> &servers)
{
    for (auto it1 = servers.begin(); it1 != servers.end(); ++it1)
    {
        for (auto it2 = std::next(it1); it2 != servers.end(); ++it2)
        {
			if (it1->port == it2->port && it1->serverName == it2->serverName)
				throw std::runtime_error("Server config file is invalid: Servers on the same port MUST have different configurations");
        }
    }
    return 0; 
}
/*
    Port numbers 1024 - 65535 are available for the following user applications:
    Port numbers 1024 - 49151 are reserved for user server applications.
    Port numbers 49152 - 65535 are reserved for clients.
    If you have a server in the client port range (49152 - 65535), define that server in the TCP/IP network services database to prevent port number conflicts.
*/
void validateServers(const std::vector<struct Server> &servers) //if there is any valid server it should run 
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i].port < 1024 && servers[i].port > 49151)
            throw std::runtime_error("Server config file is invalid: Invalid port");
        if (servers[i].clientMaxBodySize <= 0 && servers[i].clientMaxBodySize <= 6144)
            throw std::runtime_error("Server config file is invalid: The max client body size must be less that 6144 bytes and greater that 0 bytes");
        if (servers[i].socketFD < 0)
            throw std::runtime_error("Server config file is invalid: Socket file descriptor invaild");
		if (servers[i].uploadDirectory.empty() && isValidDirectory(servers[i].uploadDirectory))
            throw std::runtime_error("Server config file is invalid: Must have a default uploads path");
        if (validateErrorPage(servers[i]))
			throw std::runtime_error("Server config file is invalid: Error pages invaild");
        if (validateLocation(servers[i]))
			throw std::runtime_error("Server config file is invalid: Locations invaild");
		if (validateServerConfigurations(servers))
			throw std::runtime_error("Server config file is invalid: Servers on the same port MUST have different configurations");
    }
    std::cout << COLOR_GREEN << "Servers are valid" << COLOR_RESET << std::endl;
}