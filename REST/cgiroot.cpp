#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include <Wt/WText>
#include "cgiroot.hpp"

#include <CoreLib/log.hpp>

using namespace Wt;
using namespace StockMarket;

CgiRoot::CgiRoot(const WEnvironment &env) : WApplication(env)
{
    root()->clear();
    root()->addWidget(new WText(
                          "<center>"
                          "<h3>Iran Stock Market's RESTful web service version 1.0</h3>"
                          "</center>"
                          ));
}

WApplication *CgiRoot::CreateApplication(const WEnvironment &env)
{
    return new CgiRoot(env);
}

