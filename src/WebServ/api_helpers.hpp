#ifndef API_HELPERS_HPP
# define API_HELPERS_HPP

# include "constants.hpp"
# include <iostream>
# include <fstream>
# include <string>

// General
std::string	readFile(const std::string& path, int* status);
std::string getFileExtension(const std::string& fileName);
std::string extractPath(const std::string& request);

// Check permissions


#endif // !API_HELPERS_HPP
