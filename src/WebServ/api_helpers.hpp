#ifndef API_HELPERS_HPP
# define API_HELPERS_HPP

# include "constants.hpp"
# include <string>

FileType getFileType(const std::string& extension);
std::string extractPath(const std::string& request);
std::string getFileExtension(const std::string& fileName);

#endif // !API_HELPERS_HPP
