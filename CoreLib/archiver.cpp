#include <boost/format.hpp>
#include "archiver.hpp"
#include "system.hpp"

using namespace std;
using namespace boost;
using namespace CoreLib;

bool Archiver::UnZip(const std::string &archive, const std::string &extractionPath)
{
    string err;
    return UnZip(archive, extractionPath, err);
}

bool Archiver::UnZip(const std::string &archive, const std::string &extractionPath,
                     std::string &out_error)
{
    out_error.clear();

    std::string cmd((boost::format("%1% -o -d %2% %3%")
                     % UNZIP_BINARY
                     % extractionPath
                     % archive).str());

    bool rc = System::Exec(cmd);

    if (!rc) {
        out_error.assign((boost::format("Archiver::UnZip: Could not extract '%1%' in %2%. ")
                          % archive % extractionPath).str());
        return false;
    }

    return true;
}

