#include <chrono>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Utils>
#include <Wt/WString>
#include <CoreLib/crypto.hpp>
#include <CoreLib/db.hpp>
#include <CoreLib/log.hpp>
#include <CoreLib/make_unique.hpp>
#include "publicapiresource.hpp"
#include "dbtables.hpp"
#include "jsonexception.hpp"
#include "rt.hpp"
#include "servicecontract.hpp"
#include "xmlexception.hpp"

#define     MAX_TOKEN_MILLISECONDS_DIFFERENCE             10000

#define     INVALID_TOKEN_ERROR                      L"INVALID_TOKEN"

#define     DataByDateJSON_URI_TEMPLATE              L"StockMarket/DataByDate/JSON/{DATE}/{TOKEN}"
#define     DataByDateXML_URI_TEMPLATE               L"StockMarket/DataByDate/XML/{DATE}/{TOKEN}"
#define     LatestDataJSON_URI_TEMPLATE              L"StockMarket/LatestData/JSON/{TOKEN}"
#define     LatestDataXML_URI_TEMPLATE               L"StockMarket/LatestData/XML/{TOKEN}"
#define     TokenJSON_URI_TEMPLATE                   L"StockMarket/Token/JSON"
#define     TokenXML_URI_TEMPLATE                    L"StockMarket/Token/XML"

using namespace std;
using namespace boost;
using namespace Wt;
using namespace StockMarket;

struct PublicAPIResource::Impl
{
    typedef vector<vector<std::string> > Table_t;
    typedef vector<std::string> Row_t;

    enum class OutputType : unsigned char {
        JSON,
        XML
    };

    std::unique_ptr<StockMarket::ServiceContract> ServiceContractPtr;

    bool IsValidToken(const std::wstring &encryptedToken);

    void GetDataTree(const OutputType &outputType,
                     const std::string &dateId, const std::string &time,
                     const Row_t &titles, const Table_t &data,
                     boost::property_tree::wptree &out_tree);
    void GetDataByDate(const OutputType &outputType,
                       const std::string &date, boost::property_tree::wptree &out_tree);
    void GetLatestData(const OutputType &outputType, boost::property_tree::wptree &out_tree);
    void GetToken(boost::property_tree::wptree &out_tree);

    void DataByDateJSON(const std::wstring &date, std::wstring &out_response);
    void DataByDateXML(const std::wstring &date, std::wstring &out_response);
    void LatestDataJSON(std::wstring &out_response);
    void LatestDataXML(std::wstring &out_response);
    void TokenJSON(std::wstring &out_response);
    void TokenXML(std::wstring &out_response);

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
    m_pimpl->ServiceContractPtr->Register(TokenJSON_URI_TEMPLATE);
    m_pimpl->ServiceContractPtr->Register(TokenXML_URI_TEMPLATE);
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

            /// Validating the token
            if (uriTemplate == DataByDateJSON_URI_TEMPLATE
                    || uriTemplate == DataByDateXML_URI_TEMPLATE
                    || uriTemplate == LatestDataJSON_URI_TEMPLATE
                    || uriTemplate == LatestDataXML_URI_TEMPLATE) {
                if (!m_pimpl->IsValidToken(args[args.size() - 1])) {
                    if (boost::algorithm::contains(uriTemplate, L"/JSON")) {
                        throw StockMarket::JSONException(INVALID_TOKEN_ERROR);
                    } else if (boost::algorithm::contains(uriTemplate, L"/XML")) {
                        throw StockMarket::XMLException(INVALID_TOKEN_ERROR);
                    } else {
                        throw StockMarket::Exception(INVALID_TOKEN_ERROR);
                    }
                }
            }

            if (uriTemplate == DataByDateJSON_URI_TEMPLATE) {

                m_pimpl->DataByDateJSON(args[0], outResponse);
                PrintJSON(response, outResponse);

            } else if (uriTemplate == DataByDateXML_URI_TEMPLATE) {

                m_pimpl->DataByDateXML(args[0], outResponse);
                PrintXML(response, outResponse);

            } else if (uriTemplate == LatestDataJSON_URI_TEMPLATE) {

                m_pimpl->LatestDataJSON(outResponse);
                PrintJSON(response, outResponse);

            } else if (uriTemplate == LatestDataXML_URI_TEMPLATE) {

                m_pimpl->LatestDataXML(outResponse);
                PrintXML(response, outResponse);

            } else if (uriTemplate == TokenJSON_URI_TEMPLATE) {

                m_pimpl->TokenJSON(outResponse);
                PrintJSON(response, outResponse);

            } else if (uriTemplate == TokenXML_URI_TEMPLATE) {

                m_pimpl->TokenXML(outResponse);
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

    catch (const StockMarket::JSONException &ex) {
        PrintJSON(response, ex.What());
    }

    catch (const StockMarket::XMLException &ex) {
        PrintXML(response, ex.What());
    }

    catch (const StockMarket::Exception &ex) {
        Print(response, ex.What());
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

bool PublicAPIResource::Impl::IsValidToken(const std::wstring &encryptedToken)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    time_t token;

    try {
        std::string decryptedToken;
        RT::TokenClient()->Decrypt(WString(encryptedToken).toUTF8(), decryptedToken);
        token = boost::lexical_cast<time_t>(decryptedToken);
    } catch (...) {
        return false;
    }

    if (std::abs(std::chrono::system_clock::to_time_t(now) - token)
            > MAX_TOKEN_MILLISECONDS_DIFFERENCE) {
        return false;
    }

    return true;
}

void PublicAPIResource::Impl::GetDataTree(const OutputType &outputType,
                                          const std::string &date, const std::string &time,
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

        switch (outputType) {
        case OutputType::JSON:
            titlesTree.push_back(std::make_pair(L"", nameTree));
            break;
        case OutputType::XML:
            titlesTree.add_child(L"n", nameTree);
            break;
        }
    }
    out_tree.add_child(L"StockMarket.titles", titlesTree);

    boost::property_tree::wptree dataTree;
    for (Table_t::const_iterator it = data.begin(); it != data.end(); ++it) {
        boost::property_tree::wptree rowTree;
        for (Row_t::const_iterator rowIt = (*it).begin(); rowIt != (*it).end(); ++rowIt) {
            boost::property_tree::wptree colTree;
            colTree.put(L"", WString(*rowIt).value());

            switch (outputType) {
            case OutputType::JSON:
                rowTree.push_back(std::make_pair(L"", colTree));
                break;
            case OutputType::XML:
                rowTree.add_child(L"c", colTree);
                break;
            }
        }

        switch (outputType) {
        case OutputType::JSON:
            dataTree.push_back(std::make_pair(L"", rowTree));
            break;
        case OutputType::XML:
            dataTree.add_child(L"r", rowTree);
            break;
        }
    }
    out_tree.add_child(L"StockMarket.data", dataTree);
}

void PublicAPIResource::Impl::GetDataByDate(const OutputType &outputType, const std::string &dateId,
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
        out_tree.put(L"status", CoreLib::HTTPStatus::GetHTTPResponse(
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

    GetDataTree(outputType, date, time, titles, data, out_tree);
}

void PublicAPIResource::Impl::GetLatestData(const OutputType &outputType,
                                            boost::property_tree::wptree &out_tree)
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

    GetDataTree(outputType, date, time, titles, data, out_tree);
}

void PublicAPIResource::Impl::GetToken(boost::property_tree::wptree &out_tree)
{
    out_tree.clear();

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::duration<size_t, std::milli> millisecondsSinceEpoch =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    std::string token;
    RT::TokenServer()->Encrypt(lexical_cast<std::string>(millisecondsSinceEpoch.count()), token);

    out_tree.put(L"token", WString(token).value());
}

void PublicAPIResource::Impl::DataByDateJSON(const std::wstring &date, std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetDataByDate(OutputType::JSON, WString(date).toUTF8(), tree);

    boost::property_tree::write_json(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::DataByDateXML(const std::wstring &date, std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetDataByDate(OutputType::XML, WString(date).toUTF8(), tree);

    boost::property_tree::write_xml(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::LatestDataJSON(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetLatestData(OutputType::JSON, tree);

    boost::property_tree::write_json(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::LatestDataXML(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetLatestData(OutputType::XML, tree);

    boost::property_tree::write_xml(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::TokenJSON(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetToken(tree);

    boost::property_tree::write_json(stream, tree);

    out_response.assign(stream.str());
}

void PublicAPIResource::Impl::TokenXML(std::wstring &out_response)
{
    out_response.clear();

    std::wstringstream stream;
    boost::property_tree::wptree tree;

    GetToken(tree);

    boost::property_tree::write_xml(stream, tree);

    out_response.assign(stream.str());
}

