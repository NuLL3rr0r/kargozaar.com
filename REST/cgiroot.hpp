#ifndef CGIROOT_HPP
#define CGIROOT_HPP


#include <memory>
#include <Wt/WApplication>

namespace Wt {
    class WEnvironment;
}

namespace StockMarket {
    class CgiEnv;
    class CgiRoot;
}

class StockMarket::CgiRoot : public Wt::WApplication
{
public:
    typedef std::shared_ptr<StockMarket::CgiEnv> CgiEnvPtr_t;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    Wt::WContainerWidget *HtmlRoot;
    CgiEnvPtr_t CgiEnvInstance;

public:
    static Wt::WApplication *CreateApplication(const Wt::WEnvironment &env);

public:
    explicit CgiRoot(const Wt::WEnvironment &env);

public:
    void Redirect(const std::string &url);
    void Exit(const std::string &url);
    void Exit();
};


#endif /* CGIROOT_HPP */


