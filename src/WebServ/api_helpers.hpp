#ifndef API_HELPERS_HPP
# define API_HELPERS_HPP

# include "constants.hpp"
# include <iostream>
# include <fstream>
# include <string>

// General
std::string	readFile(std::string const & path, int* status);
std::string getFileExtension(std::string const  & fileName);
std::string extractPath(std::string const & request);

// Check permissions


#endif // !API_HELPERS_HPP
