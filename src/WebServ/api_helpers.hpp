#ifndef API_HELPERS_HPP
# define API_HELPERS_HPP

# include "constants.hpp"
# include <iostream>
# include <fstream>
# include <string>

std::string getFileExtension(std::string const  & fileName);

void		appendStatus(std::string& _res, int status);
void		appendMisc(std::string& _res);
void		appendBody(std::string& _res, std::string& body, std::string const & path);

std::string extractPath(std::string const & request);
std::string	readFile(std::string const & path);
void		checkRequest(int* status, std::string const & path);

std::string	createRedirHeader(clientData& client);
std::string	chooseErrorPage(int status);

#endif // !API_HELPERS_HPP
