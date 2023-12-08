
#include <string>
#include <sstream>

template <typename T>
inline std::string toString(const T& value)
{
    std::ostringstream oStrStream;
    oStrStream << value;
    return oStrStream.str();
}