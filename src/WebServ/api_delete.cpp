
#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "Color.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <iostream>
#include <filesystem>

/*The DELETE method requests that the origin server delete the resource identified by the Request-URI. 
	This method MAY be overridden by human intervention (or other means) on the origin server. The client cannot be guaranteed that the operation has been carried out, 
	even if the status code returned from the origin server indicates that the action has been completed successfully. However, the server SHOULD NOT indicate success unless, at the time the response is given, 
	it intends to delete the resource or move it to an inaccessible location. 
	
	A successful response SHOULD be 200 (OK) if the response includes an entity describing the status) √
	202 (Accepted) if the action has not yet been enacted √
	204 (No Content) if the action has been enacted but the response does not include an entity √
	
	If the request passes through a cache and the Request-URI identifies one or more currently cached entities, 
	those entries SHOULD be treated as stale. Responses to this method are not cacheable.
*/


#include <sys/stat.h>

void WebServerProg::deleteResponse(int clientSocket)
{
	std::string&	response = accessClientData(clientSocket)._response;
    int 			status = OK;
    std::string 	body = "";
    std::string		path;

    path = accessDataInMap(clientSocket, "Path");
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0)
    {
        if (S_ISREG(fileStat.st_mode))
        {
            if (remove(path.c_str()) == 0)
                status = OK;
            else
                status = NO_CONTENT;
        }
        else
            status = NOT_FOUND;
    }
    else
        status = NOT_FOUND;
    if (status >= ERRORS)
    {
        char buffer[1024] = {};
        path = chooseErrorPage(status);
        path = getcwd(buffer, sizeof(buffer)) + path;
    }

    appendStatus(response, status);
    appendBody(response, body, path);
}
