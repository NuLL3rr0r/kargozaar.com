#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WLink>
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

    Div* divSocialMedia = new Div(divHeader, "divSocialMedia");
    WImage *facebookImage = new WImage(WLink("facebook.png"), L"Follow us on Facebook");
    WAnchor *facebookAnchor = new WAnchor(WLink("http://www.facebook.com/kargozaar"),
                                          facebookImage, divSocialMedia);
    (void)facebookAnchor;
    WImage *twitterImage = new WImage(WLink("twitter.png"), L"Follow us on twitter");
    WAnchor *twitterAnchor = new WAnchor(WLink("http://www.twitter.com/kargozaar"),
                                         twitterImage, divSocialMedia);
    (void)twitterAnchor;

    Div* divLogo = new Div(divHeader, "divLogo");
    WImage *logoImage = new WImage(WLink("logo.png"), L"کارگزار");
    WAnchor *logoAnchor = new WAnchor(WLink("."), logoImage, divLogo);
    (void)logoAnchor;

    Div *divSeparator01 = new Div(root, "divSeparator01", "divHorizontalSeparator");
    (void)divSeparator01;

    Div *divSplash = new Div(root, "divSplash");
    Div *divSplashInner = new Div(divSplash, "divSplashInner");
    Div *divSplashPic = new Div(divSplashInner, "divSplashPic");
    (void)divSplashPic;
    Div *divSplashText = new Div(divSplashInner, "divSplashText");
    (void)divSplashText;

    Div *divSeparator02 = new Div(root, "divSeparator02", "divHorizontalSeparator");
    (void)divSeparator02;

    Div *divDownloads = new Div(root, "divDownloads");
    (void)divDownloads;

    Div *divSeparator03 = new Div(root, "divSeparator03", "divHorizontalSeparator");
    (void)divSeparator03;

    Div *divStockTable = new Div(root, "divStockTable");
    (void)divStockTable;

    return root;
}

