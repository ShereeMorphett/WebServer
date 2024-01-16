
// #include "WebServerProg.hpp"
// #include "../Color.hpp"
// #include "constants.hpp"
// #include <string>
// #include <iostream>
// #include <dirent.h>
// #include <sys/types.h>


// static int getHierarchyLevel(const std::string &path)
// {

//     int count = 0;
//     for (char c : path)
//     {
//         if (c == '/')
//             count++;
//     }
//     return count;
// }

// std::string WebServerProg::createDirectoryListing(std::string path)
// {
//     _response.append(HTTP_HEADER);
//     _response.append(NEW_VALUE);
//     _response.append("OK");
//     _response.append(NEW_VALUE);
//     _response.append(TYPE_HTML);
//     _response.append(NEW_VALUE);

//     std::string directoryFinding;
//     DIR *directory;
//     struct dirent *en;

//     directory = opendir(".");

//     if (directory == NULL)
//     {
//         std::cerr << COLOR_RED << "Error: could not open " << path << COLOR_RESET << std::endl;
//         return "";
//     }
//     if (directory)
//     {
//         while ((en = readdir(directory)) != NULL)
//         {
//             std::string entryName = en->d_name;
//             if (entryName != "." && entryName != ".." && entryName[0] != '.')
//             {
//                 // Check if the entry is a directory
//                 std::string entryPath = path + "/" + entryName;
//                 bool isDirectory = (en->d_type == DT_DIR);
//                 // Add indentation based on the hierarchy level
//                 std::string indentation = "";
//                 for (int i = 0; i < getHierarchyLevel(entryPath); ++i)
//                 {
//                     indentation += "&emsp;"; // You can adjust the indentation style
//                 }
//                 std::string link = indentation + "<a href='" + entryName + "'>" + entryName + "</a>";
//                 directoryFinding += link;
//                 if (isDirectory)
//                 {
//                     directoryFinding += " (Directory)";
//                 }
//                 directoryFinding += "<br>";
//             }
//         }
//         closedir(directory);
//     }
//     _response.append("Content-Length:" + std::to_string(directoryFinding.length()));
//     _response.append(END_HEADER);
//     _response.append(directoryFinding);
//     return _response;
// }
#include "WebServerProg.hpp"
#include "../Color.hpp"
#include "constants.hpp"
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>


std::string WebServerProg::createDirectoryListing(std::string path)
{
    _response.append(HTTP_HEADER);
    _response.append(NEW_VALUE);
    _response.append("OK");
    _response.append(NEW_VALUE);
    _response.append(TYPE_HTML);
    _response.append(NEW_VALUE);

    std::string directoryFinding;
    DIR *directory;
    struct dirent *en;

    directory = opendir(".");

    if (directory == NULL)
    {
        std::cerr << COLOR_RED << "Error: could not open " << path << COLOR_RESET << std::endl;
        return "";
    }
    if (directory)
    {
        while ((en = readdir(directory)) != NULL)
        {
            std::string entryName = en->d_name;
            if (entryName != "." && entryName != ".." && entryName[0] != '.')
            {
                // Check if the entry is a directory
                bool isDirectory = (en->d_type == DT_DIR);
				if (!isDirectory)
                {
                    directoryFinding += " &emsp;";
                }
                std::string entryPath = path + "/" + entryName;
                std::string link = "<a href='" + entryName + "'>" + entryName + "</a>";
                directoryFinding += link;
				directoryFinding += "<br>";
            }
        }
        closedir(directory);
    }
    _response.append("Content-Length:" + std::to_string(directoryFinding.length()));
    _response.append(END_HEADER);
    _response.append(directoryFinding);
    return _response;
}
