#include "Webserver.hpp"
#include "../Color.hpp"
#include <string>



std::string WebServerProg::createDirectoryListing()
{
    _response.append(HTTP_HEADER + NEW_VALUE + "OK" + NEW_VALUE + TYPE_HTML + NEW_VALUE);

    std::string directoryFinding;

    _response.append("Content-Length:" + directoryFinding.length());
    _response.append(END_HEADER);
    _response.append(directoryFinding);


    return _response;
}