#ifndef ARCHIVER_HPP
#define ARCHIVER_HPP


#include <string>

namespace CoreLib {
    class Archiver;
}

class CoreLib::Archiver
{
public:
    static bool UnZip(const std::string &archive, const std::string &extractionPath);
    static bool UnZip(const std::string &archive, const std::string &extractionPath,
                      std::string &out_error);
};


#endif /* ARCHIVER_HPP */

