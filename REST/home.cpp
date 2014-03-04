#include <boost/any.hpp>
#include <boost/format.hpp>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WLink>
#include <Wt/WObject>
#include <Wt/WSignalMapper>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTableView>
#include <Wt/WText>
#include <Wt/WTimer>
#include <Wt/WWidget>
#include <CoreLib/cdate.hpp>
#include <CoreLib/db.hpp>
#include <CoreLib/log.hpp>
#include <CoreLib/make_unique.hpp>
#include "home.hpp"
#include "dbtables.hpp"
#include "div.hpp"
#include "rt.hpp"

#define         STOCK_DATA_TABLE_REFRESH_INTERVAL       10000

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
#define         STOCK_DATA_LINK_TEXT    L"جدول آخرین تغییرات بازار بورس"

using namespace std;
using namespace Wt;
using namespace StockMarket;

class Home::Impl : public Wt::WObject
{
public:
    bool IsStockDataTableVisible;
    Wt::WTimer *StockDataTableRefreshTimer;
    Div *DivStockTableInner;

public:
    Impl();
    ~Impl();

public:
    void OnStockDataTableLinkTextClicked();
    void RefreshStockDataTable();
};

Home::Home(CgiRoot *cgi) :
    Page(cgi),
    m_pimpl(std::make_unique<Home::Impl>())
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

    WText *stockDataTableLinkText = new WText(WString(L"<h3>{1}</h3>").arg(std::wstring(STOCK_DATA_LINK_TEXT)),
                                              divStockTable);
    stockDataTableLinkText->setStyleClass("link");

    m_pimpl->DivStockTableInner = new Div(divStockTable, "divStockTableInner");

    Div *divCopyright = new Div(root, "divCopyright");
    CDate::Now n;
    new WText(L"&copy; " + CDate::DateConv::FormatToPersianNums(boost::lexical_cast<string>(n.Year))
              + L" کارگزار، تمامی حقوق محفوظ است.", divCopyright);

    WSignalMapper<WText *> *stockDataTableLinkSignalMapper = new WSignalMapper<WText *>(m_pimpl.get());
    stockDataTableLinkSignalMapper->mapped().connect(m_pimpl.get(), &Home::Impl::OnStockDataTableLinkTextClicked);
    stockDataTableLinkSignalMapper->mapConnect(stockDataTableLinkText->clicked(), stockDataTableLinkText);

    return root;
}

Home::Impl::Impl() :
    IsStockDataTableVisible(false)
{
    StockDataTableRefreshTimer = new Wt::WTimer();
    StockDataTableRefreshTimer->setInterval(STOCK_DATA_TABLE_REFRESH_INTERVAL);
    StockDataTableRefreshTimer->timeout().connect(this, &Home::Impl::RefreshStockDataTable);
}

Home::Impl::~Impl()
{

}

void Home::Impl::OnStockDataTableLinkTextClicked()
{
    if (IsStockDataTableVisible) {
        StockDataTableRefreshTimer->stop();
        DivStockTableInner->clear();
        IsStockDataTableVisible = false;
        return;
    } else {
        IsStockDataTableVisible = true;
        RefreshStockDataTable();
        StockDataTableRefreshTimer->start();
    }
}

void Home::Impl::RefreshStockDataTable()
{
    DivStockTableInner->clear();

    Div *root = new Div(DivStockTableInner, "divStockDataTable");

    Div *divLastUpdateDate = new Div(root, "divLastUpdateDate");

    WStandardItemModel *model = new WStandardItemModel();

    cppdb::transaction guard(RT::DB()->SQL());

    cppdb::result r = RT::DB()->SQL()
            << (boost::format("SELECT date, time"
                              " FROM %1%"
                              " ORDER BY ROWID ASC"
                              " LIMIT 1;")
                % RT::DBTables()->Table("LAST_UPDATE")).str()
            << cppdb::row;

    if (!r.empty()) {
        std::string date;
        std::string time;

        r >> date >> time;

        new WText(WString(L"زمان آخرین تغییرات: {1} - {2}").arg(date).arg(time), divLastUpdateDate);
    }

    r = RT::DB()->SQL()
            << (boost::format("SELECT title"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % RT::DBTables()->Table("DATA_TITLES")).str();

    int col = 0;
    while(r.next()) {
        std::string value;

        r >> value;

        model->insertColumns(model->columnCount(), col + 1 - model->columnCount());
        model->setHeaderData(col, boost::any(Wt::WString::fromUTF8(value)));
        ++col;
    }

    r = RT::DB()->SQL()
            << (boost::format("SELECT *"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % RT::DBTables()->Table("STOCK_DATA")).str();

    while(r.next()) {
        vector<WStandardItem *> row;

        for (int i = 0; i < r.cols(); ++i) {
            string value;

            r >> value;

            // skip the 'r' column
            if (i == 0)
                continue;

            WStandardItem *item = new WStandardItem(WString::fromUTF8(value));
            row.push_back(item);
        }
        model->appendRow(row);
    }

    guard.rollback();

    Wt::WTableView *tableView = new WTableView(root);
    tableView->setModel(model);
    tableView->setColumnResizeEnabled(false);
    tableView->setColumnAlignment(0, Wt::AlignCenter);
    tableView->setHeaderAlignment(0, Wt::AlignCenter);
    tableView->setAlternatingRowColors(true);
    tableView->setRowHeight(28);
    tableView->setHeaderHeight(28);
    tableView->setSelectionMode(Wt::SingleSelection);
    tableView->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
}

