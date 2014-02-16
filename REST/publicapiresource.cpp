#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Utils>
#include <Wt/WString>
#include <CoreLib/db.hpp>
#include <CoreLib/log.hpp>
#include <CoreLib/make_unique.hpp>
#include "publicapiresource.hpp"
#include "dbtables.hpp"
#include "rt.hpp"
#include "servicecontract.hpp"

#define     DataByDateJSON_URI_TEMPLATE              L"StockMarket/DataByDate/JSON/{DATE}"
#define     DataByDateXML_URI_TEMPLATE               L"StockMarket/DataByDate/XML/{DATE}"
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

    std::unique_ptr<StockMarket::ServiceContract> ServiceContractPtr;

    void GetDataTree(const std::string &dateId, const std::string &time,
                     const Row_t &titles, const Table_t &data,
                     boost::property_tree::wptree &out_tree);
    void GetDataByDate(const std::string &date, boost::property_tree::wptree &out_tree);
    void GetLatestData(boost::property_tree::wptree &out_tree);

    void DataByDateJSON(const std::wstring &date, std::wstring &out_response);
    void DataByDateXML(const std::wstring &date, std::wstring &out_response);
    void LatestDataJSON(std::wstring &out_response);
    void LatestDataXML(std::wstring &out_response);

    Impl();
    ~Impl();
};


PublicAPIResource::PublicAPIResource(WObject *parent) :
    APIResource(parent),
    m_pimpl(std::make_unique<PublicAPIResource::Impl>())
{
    m_pimpl->ServiceContractPtr = std::make_unique<StockMarket::ServiceContract>();
    m_pimpl->ServiceContractPtr->Register(DataByDateJSON_URI_TEMPLATE);
    m_pimpl->ServiceContractPtr->Register(DataByDateXML_URI_TEMPLATE);
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

            if (uriTemplate == DataByDateJSON_URI_TEMPLATE) {

                m_pimpl->DataByDateJSON(args[0], outResponse);
                PrintJSON(response, outResponse);

            } else if (uriTemplate == DataByDateJSON_URI_TEMPLATE) {

                m_pimpl->DataByDateXML(args[0], outResponse);
                PrintJSON(response, outResponse);

            } else if (uriTemplate == LatestDataJSON_URI_TEMPLATE) {

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

void PublicAPIResource::Impl::GetDataTree(const std::string &date, const std::string &time,
                                          const Row_t &titles, const Table_t &data,
                                          boost::property_tree::wptree &out_tree)
{
    out_tree.put(L"StockMarket.version.major", "1");
    out_tree.put(L"StockMarket.version.minor", "0");
    out_tree.put(L"StockMarket.version.compat.major", "1");
    out_tree.put(L"StockMarket.version.compat.minor", "0");

    out_tree.put(L"StockMarket.date", WString(date).value());
    out_tree.put(L"StockMarket.time", WString(time).value());

    boost::property_tree::wptree titlesTree;
    for (Row_t::const_iterator it = titles.begin(); it != titles.end(); ++it) {
        boost::property_tree::wptree nameTree;
        nameTree.put(L"", WString(*it).value());
        titlesTree.add_child(L"n", nameTree);
    }
    out_tree.add_child(L"StockMarket.titles", titlesTree);

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
    out_tree.add_child(L"StockMarket.data", dataTree);
}

void PublicAPIResource::Impl::GetDataByDate(const std::string &dateId,
                                            boost::property_tree::wptree &out_tree)
{
    out_tree.clear();

    std::string date;
    std::string time;
    std::string dataTitlesTable;
    std::string stockDataTable;
    Row_t titles;
    Table_t data;

    cppdb::transaction guard(RT::DB()->SQL());

    cppdb::result r = RT::DB()->SQL()
            << (boost::format("SELECT date, time, datatitlestbl, stockdatatbl"
                              " FROM %1%"
                              " WHERE date=? "
                              " ORDER BY ROWID ASC"
                              " LIMIT 1;")
                % RT::DBTables()->Table("ARCHIVE")).str()
            << boost::replace_all_copy(dateId, "-", "/")
            << cppdb::row;

    if (r.empty()) {
        out_tree.put(L"error", CoreLib::HTTPStatus::GetHTTPResponse(
                         CoreLib::HTTPStatus::HTTPStatusCode::HTTP_404));
        return;
    }

    r >> date >> time >> dataTitlesTable >> stockDataTable;

    r = RT::DB()->SQL()
            << (boost::format("SELECT title"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % dataTitlesTable).str();

    std::string value;
    while(r.next()) {
        r >> value;
        titles.push_back(value);
    }

    r = RT::DB()->SQL()
            << (boost::format("SELECT *"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % stockDataTable).str();

    size_t rowId = 0;
    while(r.next()) {
        Row_t row;
        data.push_back(row);
        for (int i = 0; i < r.cols(); ++i) {
            r >> value;

            // skip the 'r' column
            if (i == 0)
                continue;

            data[rowId].push_back(value);
        }
        ++rowId;
    }

    guard.rollback();

    GetDataTree(date, time, titles, data, out_tree);
}

void PublicAPIResource::Impl::GetLatestData(boost::property_tree::wptree &out_tree)
{
    out_tree.clear();

    std::string date;
    std::string time;
    Row_t titles;
    Table_t data;

    cppdb::transaction guard(RT::DB()->SQL());

    cppdb::result r = RT::DB()->SQL()
            << (boost::format("SELECT date, time"
                              " FROM %1%"
                              " ORDER BY ROWID ASC"
                              " LIMIT 1;")
                % RT::DBTables()->Table("LAST_UPDATE")).str()
            << cppdb::row;

    if (r.empty()) {
        return;
    }

    r >> date >> time;

    r = RT::DB()->SQL()
            << (boost::format("SELECT title"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % RT::DBTables()->Table("DATA_TITLES")).str();

    std::string value;
    while(r.next()) {
        r >> value;
        titles.push_back(value);
    }

    r = RT::DB()->SQL()
            << (boost::format("SELECT *"
                              " FROM %1%"
                              " ORDER BY ROWID ASC;")
                % RT::DBTables()->Table("STOCK_DATA")).str();

    size_t rowId = 0;
    while(r.next()) {
        Row_t row;
        data.push_back(row);
        for (int i = 0; i < r.cols(); ++i) {
            r >> value;

            // skip the 'r' column
            if (i == 0)
                continue;

            data[rowId].push_back(value);
        }
        ++rowId;
    }

    guard.rollback();

    GetDataTree(date, time, titles, data, out_tree);
}

void PublicAPIResource::Impl::DataByDateJSON(const std::wstring &date, std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetDataByDate(WString(date).toUTF8(), tree);

    boost::property_tree::write_json(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::DataByDateXML(const std::wstring &date, std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetDataByDate(WString(date).toUTF8(), tree);

    boost::property_tree::write_xml(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::LatestDataJSON(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetLatestData(tree);

    boost::property_tree::write_json(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::LatestDataXML(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetLatestData(tree);

    boost::property_tree::write_xml(stream, tree);

    out_response.assign(stream.str());
}

