#include "exception.hpp"

using namespace std;
using namespace CoreLib;

Exception::Exception(const string &message)
    : runtime_error(message)
{
}

