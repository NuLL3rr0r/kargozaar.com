#ifndef IO_HPP
#define IO_HPP


#include <string>

namespace CoreLib {
    class IO;
}

class CoreLib::IO
{
public:
    static bool DirExists(const std::string &path);
    static bool FileExists(const std::string &path);
    static std::size_t FileSize(const std::string &path);

    static bool EraseDir(const std::string &path);
    static bool EraseFile(const std::string &path);
};


#endif /* IO_HPP */


