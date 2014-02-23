#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include <Wt/WText>
#include <CoreLib/log.hpp>
#include <CoreLib/make_unique.hpp>
#include "cgiroot.hpp"
#include "cgienv.hpp"
#include "exception.hpp"
#include "home.hpp"

#define         UNKNOWN_ERROR       "CgiRoot::CgiRoot ** Uknown error!"
#define         ALICE               L"<pre>Alice is not in Wonderland!!</pre>"

using namespace std;
using namespace Wt;
using namespace StockMarket;

struct CgiRoot::Impl
{
private:
    CgiRoot *m_cgiRoot;

public:
    Impl(CgiRoot *cgiRoot);
    ~Impl();

    Wt::WWidget *GetHomePage();
};

WApplication *CgiRoot::CreateApplication(const WEnvironment &env)
{
    return new CgiRoot(env);
}

CgiRoot::CgiRoot(const WEnvironment &env) :
    WApplication(env),
    m_pimpl(std::make_unique<CgiRoot::Impl>(this))
{
    try {
        this->setInternalPathDefaultValid(false);

        setCssTheme("bootstrap");

        CgiEnvInstance = std::make_shared<CgiEnv>(env);
        if (CgiEnvInstance->FoundXSS())
            throw StockMarket::Exception(ALICE);

        root()->clear();
        HtmlRoot = root();

        root()->addWidget(m_pimpl->GetHomePage());
    }

    catch (const StockMarket::Exception &ex) {
        root()->clear();
        root()->addWidget(new WText(ex.What()));
    }

    catch (const CoreLib::Exception &ex) {
        LOG_ERROR(ex.what());
    }

    catch (...) {
        LOG_ERROR(UNKNOWN_ERROR);
    }
}

void CgiRoot::Redirect(const std::string &url)
{
    redirect(url);
}

void CgiRoot::Exit(const std::string &url)
{
    redirect(url);
    quit();
}

void CgiRoot::Exit()
{
    quit();
}

CgiRoot::Impl::Impl(CgiRoot *cgiRoot) :
    m_cgiRoot(cgiRoot)
{

}

CgiRoot::Impl::~Impl()
{

}

Wt::WWidget *CgiRoot::Impl::GetHomePage()
{
    m_cgiRoot->useStyleSheet("home.css");

    return new Home(m_cgiRoot);
}

