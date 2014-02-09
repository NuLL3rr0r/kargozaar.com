#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP


#include <stdexcept>
#include <string>

namespace CoreLib {
    class Exception;
}

class CoreLib::Exception : public std::runtime_error
{
public:
    Exception(const std::string &message);
};


#endif /* EXCEPTION_HPP */

