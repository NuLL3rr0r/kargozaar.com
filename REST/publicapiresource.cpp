#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WString>
#include <CoreLib/db.hpp>
#include <CoreLib/log.hpp>
#include <CoreLib/make_unique.hpp>
#include "publicapiresource.hpp"
#include "dbtables.hpp"
#include "rt.hpp"
#include "servicecontract.hpp"

#define     LatestDataJSON_URI_TEMPLATE              L"StockMarket/LatestData/JSON"
#define     LatestDataXML_URI_TEMPLATE               L"StockMarket/LatestData/XML"

using namespace std;
using namespace boost;
using namespace Wt;
using namespace StockMarket;

struct PublicAPIResource::Impl
{
    typedef vector<vector<std::string> > Table_t;
    typedef vector<std::string> Row_t;

    std::unique_ptr<ServiceContract<Impl> > ServiceContractPtr;

    void GetLatestData(std::string &out_date, std::string &out_time,
                       Row_t &out_titles,
                       Table_t &out_data);

    void LatestDataJSON(std::wstring &out_response);
    void LatestDataXML(std::wstring &out_response);

    Impl();
    ~Impl();
};


PublicAPIResource::PublicAPIResource(WObject *parent) :
    APIResource(parent),
    m_pimpl(std::make_unique<PublicAPIResource::Impl>())
{
    m_pimpl->ServiceContractPtr = std::make_unique<StockMarket::ServiceContract<PublicAPIResource::Impl> >(m_pimpl.get());
    m_pimpl->ServiceContractPtr->Register(LatestDataJSON_URI_TEMPLATE);
    m_pimpl->ServiceContractPtr->Register(LatestDataXML_URI_TEMPLATE);
}

PublicAPIResource::~PublicAPIResource()
{
    beingDeleted();
}

void PublicAPIResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    try {
        WString uri((boost::format("%1%%2%")
                     % request.path().substr(request.path().find_last_of("/") + 1)
                     % request.pathInfo()
                     ).str());
        wstring uriTemplate;
        vector<wstring> args;

        if (m_pimpl->ServiceContractPtr->Resolve(uri.value(), uriTemplate, args)) {
            wstring outResponse;

            if (uriTemplate == LatestDataJSON_URI_TEMPLATE) {
                m_pimpl->LatestDataJSON(outResponse);
                PrintJSON(response, outResponse);
            } else if (uriTemplate == LatestDataXML_URI_TEMPLATE) {
                m_pimpl->LatestDataXML(outResponse);
                PrintXML(response, outResponse);
            }
        } else {
            if (boost::algorithm::contains(uri.value(), L"/JSON")) {
                PrintJSON(response, GetHTTPStatusJSON(CoreLib::HTTPStatus::HTTPStatusCode::HTTP_400));
            } else if (boost::algorithm::contains(uri.value(), L"/XML")) {
                PrintXML(response, GetHTTPStatusXML(CoreLib::HTTPStatus::HTTPStatusCode::HTTP_400));
            } else {
                Print(response, GetHTTPStatus(CoreLib::HTTPStatus::HTTPStatusCode::HTTP_400));
            }
        }
    }

    catch (...) {
        Print(response, GetHTTPStatus(CoreLib::HTTPStatus::HTTPStatusCode::HTTP_500));
    }
}

PublicAPIResource::Impl::Impl()
{

}

PublicAPIResource::Impl::~Impl()
{

}

void PublicAPIResource::Impl::GetLatestData(std::string &out_date, std::string &out_time,
                                            Row_t &out_titles,
                                            Table_t &out_data)
{
    out_date.clear();
    out_time.clear();
    out_titles.clear();
    out_data.clear();

    cppdb::transaction guard(RT::DB()->SQL());

    cppdb::result r = RT::DB()->SQL()
            << (boost::format("SELECT date, time"
                              " FROM %1%"
                              " ORDER BY ROWID ASC"
                              " LIMIT 1;")
                % RT::DBTables()->Table("LAST_UPDATE")).str()
            << cppdb::row;

    if (!r.empty()) {
        r >> out_date >> out_time;
    }

    r = RT::DB()->SQL()
            << (boost::format("SELECT title"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % RT::DBTables()->Table("DATA_TITLES")).str();

    std::string value;
    while(r.next()) {
        r >> value;
        out_titles.push_back(value);
    }

    r = RT::DB()->SQL()
            << (boost::format("SELECT *"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % RT::DBTables()->Table("STOCK_DATA")).str();

    size_t rowId = 0;
    while(r.next()) {
        Row_t row;
        out_data.push_back(row);
        for (int i = 0; i < r.cols(); ++i) {
            r >> value;

            // skip the 'r' column
            if (i == 0)
                continue;

            out_data[rowId].push_back(value);
        }
        ++rowId;
    }

    guard.rollback();
}

void PublicAPIResource::Impl::LatestDataJSON(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    std::string date;
    std::string time;
    Row_t titles;
    Table_t data;
    GetLatestData(date, time, titles, data);

    tree.put(L"StockMarket.version.major", "1");
    tree.put(L"StockMarket.version.minor", "0");
    tree.put(L"StockMarket.version.compat.minor", "1");
    tree.put(L"StockMarket.version.compat.minor", "0");

    tree.put(L"StockMarket.date", WString(date).value());
    tree.put(L"StockMarket.time", WString(time).value());

    boost::property_tree::wptree titlesTree;
    for (Row_t::const_iterator it = titles.begin(); it != titles.end(); ++it) {
        boost::property_tree::wptree nameTree;
        nameTree.put(L"", WString(*it).value());
        titlesTree.add_child(L"n", nameTree);
    }
    tree.add_child(L"StockMarket.titles", titlesTree);

    boost::property_tree::wptree dataTree;
    for (Table_t::const_iterator it = data.begin(); it != data.end(); ++it) {
        boost::property_tree::wptree rowTree;
        for (Row_t::const_iterator rowIt = (*it).begin(); rowIt != (*it).end(); ++rowIt) {
            boost::property_tree::wptree colTree;
            colTree.put(L"", WString(*rowIt).value());
            rowTree.add_child(L"c", colTree);
        }
        dataTree.add_child(L"r", rowTree);
    }
    tree.add_child(L"StockMarket.data", dataTree);

    boost::property_tree::write_json(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::LatestDataXML(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    std::string date;
    std::string time;
    Row_t titles;
    Table_t data;
    GetLatestData(date, time, titles, data);

    tree.put(L"StockMarket.version.major", "1");
    tree.put(L"StockMarket.version.minor", "0");
    tree.put(L"StockMarket.version.compat.major", "1");
    tree.put(L"StockMarket.version.compat.minor", "0");

    tree.put(L"StockMarket.date", WString(date).value());
    tree.put(L"StockMarket.time", WString(time).value());

    boost::property_tree::wptree titlesTree;
    for (Row_t::const_iterator it = titles.begin(); it != titles.end(); ++it) {
        boost::property_tree::wptree nameTree;
        nameTree.put(L"", WString(*it).value());
        titlesTree.add_child(L"n", nameTree);
    }
    tree.add_child(L"StockMarket.titles", titlesTree);

    boost::property_tree::wptree dataTree;
    for (Table_t::const_iterator it = data.begin(); it != data.end(); ++it) {
        boost::property_tree::wptree rowTree;
        for (Row_t::const_iterator rowIt = (*it).begin(); rowIt != (*it).end(); ++rowIt) {
            boost::property_tree::wptree colTree;
            colTree.put(L"", WString(*rowIt).value());
            rowTree.add_child(L"c", colTree);
        }
        dataTree.add_child(L"r", rowTree);
    }
    tree.add_child(L"StockMarket.data", dataTree);

    boost::property_tree::write_xml(stream, tree);

    out_response.assign(stream.str());
}

