#include <Wt/WText>
#include <Wt/WWidget>
#include "home.hpp"
#include "div.hpp"

#define         PAGE_TITLE              L"کارگزار"

using namespace std;
using namespace Wt;
using namespace StockMarket;

Home::Home(CgiRoot *cgi) :
    Page(cgi)
{
    m_cgiRoot->setTitle(PAGE_TITLE);

    this->clear();
    this->setId("HomePage");
    this->addWidget(Layout());
}

Home::~Home()
{

}

WWidget *Home::Layout()
{
    Div *root = new Div("Home");

    return root;
}

