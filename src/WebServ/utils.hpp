
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include "WebServerProg.hpp"


struct server;
struct location;

template <typename T>
std::string toString(const T& value)
{
    std::ostringstream oStrStream;
    oStrStream << value;
    return oStrStream.str();
}

template <typename KeyType, typename ValueType>
void printMultimap(const std::multimap<KeyType, ValueType>& myMultimap) {
    typename std::multimap<KeyType, ValueType>::const_iterator it;

    for (it = myMultimap.begin(); it != myMultimap.end(); ++it) {
        std::cout << "Key: " << it->first << "   Value: " << it->second << std::endl;
    }
}

template <typename Type,typename T>
void printMap(typename std::map<Type, T> theMap)
{
	for (typename std::map<Type, T>::iterator it = theMap.begin(); it != theMap.end(); ++it)
	{
        std::cout << it->first << ": " << it->second << std::endl;
    }
}
bool isValidDirectory(const std::string& path);
bool isValidFile(const std::string& path);
void skipNonPrintable(std::istream& stream);
void skipWhitespace(std::istream& stream);
void printLocation(location &location);
void printServer(server &server);
int	countDepth(std::string path);
std::string parseStartingPath(std::string startingPath, std::string root);