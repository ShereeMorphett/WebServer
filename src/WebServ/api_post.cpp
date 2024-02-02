#include "WebServerProg.hpp"
#include "api_helpers.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "Color.hpp"
#include <unistd.h>
#include <iomanip>//
#include <sstream>//

static void	appendMisc(std::string& _res, size_t contentSize) {
	_res.append("Content-Type: text/html");
	_res.append(NEW_VALUE);
	_res.append("Content-length: " + std::to_string(contentSize));
	_res.append(END_HEADER);
}

std::string urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }
    }

    return escaped.str();
}

static std::string generateHtmlcontentPost(clientData& client, std::string uploadPath)
{
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

	std::string html_content = "<!DOCTYPE html>\n<html>\n<head>\n<title>You Successfully uploaded!</title>\n</head><style>" + css_content + "</style>\n<body>\n<p>You Successfully uploaded! Click the link below to view your file.</p>\n";
	std::string filePath  = uploadPath+ "/" + urlEncode(client._fileName);
	html_content += "<a href=\"" + filePath + "\" target=\"_blank\">View File</a>\n";
	html_content += "<form action=\"/delete\" method=\"post\">\n";
	html_content += "<input type=\"hidden\" name=\"file\" value=\"" + filePath + "\">\n";
	html_content += R"(<input type="button" value="Delete File" onclick="deleteFile(')" + filePath + R"###(')">)###";
	html_content += R"(
	<script>
	function deleteFile(filePath) {
		const apiEndpoint = '/delete';

		fetch(filePath, {
			method: 'DELETE',
			headers: {
				'Content-Type': 'application/json',
			}
		})
		.then(response => {
			if (response.ok) {
				// Handle successful delete, e.g., redirect or update UI
				console.log('File deleted successfully');
			} else {
				// Handle error response
				console.error('Error during the DELETE request:', response.status, response.statusText);
			}
		})
		.catch(error => {
			console.error('Error during the DELETE request:', error);
		});
	}
	</script>)";

	html_content += "</body>\n</html>\r\n\r\n";
	return html_content;
}

static bool validateFilename(std::string& fileName)
{
    if (fileName.find(' ') != std::string::npos)
	{
		std::cerr << COLOR_RED << "Error: Invalid file name" << COLOR_RESET << std::endl;
		return false;
	}
	
	return true;
}

static std::string getFileUploadPath(clientData& client, std::vector<server>& servers) //will return a path
{
	// std::cout << COLOR_CYAN;
	// printMultimap(client.requestData);
	// std::cout << COLOR_RESET << std::endl;
	// std::cout << COLOR_MAGENTA << client.requestData.find("Path")->second;

	return servers[client.serverIndex].uploadFile;
	
}


void WebServerProg::postResponse(int clientSocket) {

    clientData& client = accessClientData(clientSocket);
	std::string uploadPath = getFileUploadPath(client, servers);
	if (validateFilename(client._fileName) == false)
	{
		std::string html_content = "<!DOCTYPE html>\n<html>\n<head>\n<title>Error with upload! Invalid File Name</title>\n</head>\n<body>Error with upload!\n</body>\n</html>\r\n\r\n";
		appendMisc(client._response, html_content.size());
		client._response.append(html_content);
		client._status = ERRORS;
	}
	if (client._status < ERRORS)
	{
		std::ofstream outFile(uploadPath + "/"+ client._fileName, std::ios::binary);
		if (!outFile)
			client._status = INT_ERROR;
		else {
			outFile << client._fileData;
			outFile.close();
			client._status = OK;
		}
	}
	client._response.clear();
    appendStatus(client._response, client._status);
	if (client._status == OK)
	{
        std::string html_content = generateHtmlcontentPost(client, uploadPath);
        appendMisc(client._response, html_content.size());
        client._response.append(html_content);
	}
	else
	{
		std::string html_content = "<!DOCTYPE html>\n<html>\n<head>\n<title>Error with upload!</title>\n</head>\n<body>Error with upload!\n</body>\n</html>\r\n\r\n";
		appendMisc(client._response, html_content.size());
		client._response.append(html_content);
	}
}