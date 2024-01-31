#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "Color.hpp"
#include <unistd.h>

static void	appendMisc(std::string& _res, size_t contentSize) {
	_res.append("Content-Type: text/html");
	_res.append(NEW_VALUE);
	_res.append("Content-length: " + std::to_string(contentSize));
	_res.append(END_HEADER);
}

void WebServerProg::postResponse(int clientSocket) {
    clientData& client = accessClientData(clientSocket);

	if (client._status < ERRORS) {	
		std::ofstream outFile(client._fileName, std::ios::binary);
		if (!outFile)
			client._status = INT_ERROR;
		else {
			outFile << client._fileData;
			outFile.close();
			client._status = OK;
		}
	}
	client._response.clear();//
    appendStatus(client._response, client._status);

	std::string css_content = R"(body {
        font-family: 'Arial', sans-serif;
        background-color: #f2f2f2;}h1 {
		color: rgb(0, 128, 128);
		display: inline-block; 
		margin-right: 10px; 
	}
    p {
        color: black;
        font-size: 40px;
    })";

	if (client._status == OK)
	{
    	std::string html_content = "<!DOCTYPE html>\n<html>\n<head>\n<title>You Successfully uploaded!</title>\n</head>\n<body>You Successfully uploaded!\n</body>\n</html>\r\n\r\n";
		appendMisc(client._response, html_content.size());
		client._response.append(html_content);
	}
	else
	{
		//this would probably be an error page situation
		std::string html_content = "<!DOCTYPE html>\n<html>\n<head>\n<title>Error with upload!</title>\n</head>\n<body>Error with upload!\n</body>\n</html>\r\n\r\n";
		appendMisc(client._response, html_content.size());
		client._response.append(html_content);
	}
}