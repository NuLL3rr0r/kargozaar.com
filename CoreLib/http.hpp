#ifndef HTTP_HPP
#define HTTP_HPP


#include <string>

namespace CoreLib {
    class HTTP;
}

class CoreLib::HTTP
{
public:
    static bool DownloadFile(const std::string &remoteAddr, const std::string &localPath);
    static bool DownloadFile(const std::string &remoteAddr, const std::string &localPath,
                             std::string &out_error);
};


#endif /* HTTP_HPP */

