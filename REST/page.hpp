#ifndef PAGE_HPP
#define PAGE_HPP


#include <memory>
#include <string>
#include <Wt/WContainerWidget>

namespace Wt {
    class WText;
    class WWidget;
}

#include "cgiroot.hpp"

namespace StockMarket {
    class Page;
}

class StockMarket::Page : public Wt::WContainerWidget
{
protected:
    CgiRoot *m_cgiRoot;
    Wt::WContainerWidget *m_htmlRoot;

    CgiRoot::CgiEnvPtr_t m_cgiEnv;

public:
    Page(CgiRoot *cgiRoot);
    virtual ~Page() = 0;

protected:
    void Redirect(const std::string &url);
    void Exit(const std::string &url);
    void Exit();

    bool Validate(Wt::WFormWidget *widget);
    void HtmlError(const std::string &err, Wt::WText *txt);
    void HtmlError(const std::wstring &err, Wt::WText *txt);
    void HtmlInfo(const std::string &msg, Wt::WText *txt);
    void HtmlInfo(const std::wstring &msg, Wt::WText *txt);

    virtual Wt::WWidget *Layout() = 0;
};


#endif /* PAGE_HPP */

