#include<Wt/WApplication>
#include<Wt/WFormWidget>
#include<Wt/WText>
#include <Wt/WWidget>
#include "page.hpp"

using namespace std;
using namespace Wt;
using namespace StockMarket;

Page::Page(CgiRoot *cgiRoot) :
    WContainerWidget(),
    m_cgiRoot(cgiRoot),
    m_htmlRoot(cgiRoot->HtmlRoot),
    m_cgiEnv(cgiRoot->CgiEnvInstance)
{

}

Page::~Page()
{

}

void Page::Redirect(const string& url)
{
    m_cgiRoot->Redirect(url);
}

void Page::Exit(const std::string& url)
{
    m_cgiRoot->Exit(url);
}

void Page::Exit()
{
    m_cgiRoot->Exit();
}

bool Page::Validate(WFormWidget *widget)
{
    switch (widget->validate()) {
    case WValidator::Valid:
        return true;
    case WValidator::InvalidEmpty:
        break;
    case WValidator::Invalid:
        break;
    }

    widget->setFocus();
    return false;
}

void Page::HtmlError(const string& err, WText *txt)
{
    txt->setText(err);
    txt->setStyleClass("boldTextErr");
}

void Page::HtmlError(const wstring& err, WText *txt)
{
    txt->setText(err);
    txt->setStyleClass("boldTextErr");
}

void Page::HtmlInfo(const string& msg, WText *txt)
{
    txt->setText(msg);
    txt->setStyleClass("boldText");
}

void Page::HtmlInfo(const wstring& msg, WText *txt)
{
    txt->setText(msg);
    txt->setStyleClass("boldText");
}

