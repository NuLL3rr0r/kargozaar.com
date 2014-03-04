#ifndef HOME_HPP
#define HOME_HPP


#include "page.hpp"

namespace StockMarket {
    class Home;
}

class StockMarket::Home : public StockMarket::Page
{
private:
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    Home(CgiRoot *cgiRoot);
    ~Home();

private:
    Wt::WWidget *Layout();
};


#endif /* HOME_HPP */

