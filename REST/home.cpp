#include <Wt/WText>
#include <Wt/WWidget>
#include "home.hpp"
#include "div.hpp"

#define         PAGE_TITLE              L"کارگزار"
#define         NO_SCRIPT_WARN \
    L"<noscript>" \
    "<div class=\"dvNoScript\">" \
    "کاربر گرامی به نظر می رسد که جاوا اسکریپت در مرورگر شما غیر فعال می باشد " \
    "و یا مرورگر شما از جاوا اسکریپت پشتیبانی نمی نماید. " \
    "متاسفانه در صورت عدم رفع این مشکل از سوی شما ممکن است برخی از قابلیت های " \
    "وب سایت به درستی کار نکند." \
    "</div>" \
    "</noscript>"

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

    Div *divNoScript = new Div(root, "divNoScript");
    new WText(NO_SCRIPT_WARN, divNoScript);

    Div *divHeader = new Div(root, "divHeader");
    (void)divHeader;

    Div *divSplash = new Div(root, "divSplash");
    Div *divSplashPic = new Div(divSplash, "divSplashPic");
    (void)divSplashPic;
    Div *divSplashText = new Div(divSplash, "divSplashText");
    (void)divSplashText;

    Div *divDownloads = new Div(root, "divDownloads");
    (void)divDownloads;

    Div *divStockTable = new Div(root, "divStockTable");
    (void)divStockTable;

    return root;
}

