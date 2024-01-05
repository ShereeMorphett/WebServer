#ifndef API_HELPERS_HPP
# define API_HELPERS_HPP

# include "constants.hpp"
# include <iostream>
# include <fstream>
# include <string>

// General
std::string getFileExtension(std::string const  & fileName);
void		appendStatus(std::string& _res, int status);
std::string extractPath(std::string const & request);
std::string	readFile(std::string const & path);
std::string	chooseErrorPage(int status);
void	checkRequest(int* status, std::string const & path);
void	appendBody(std::string& _res, std::string& body, std::string const & path);
void	appendStatus(std::string& _res, int status);


// Check permissions


#endif // !API_HELPERS_HPP
