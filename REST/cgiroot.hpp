#ifndef CGIROOT_HPP
#define CGIROOT_HPP


#include <Wt/WApplication>

namespace Wt {
    class WEnvironment;
}

namespace StockMarket {
    class CgiRoot;
}

class StockMarket::CgiRoot : public Wt::WApplication
{
public:
    CgiRoot(const Wt::WEnvironment &env);

public:
    static Wt::WApplication *CreateApplication(const Wt::WEnvironment &env);
};


#endif /* CGIROOT_HPP */


