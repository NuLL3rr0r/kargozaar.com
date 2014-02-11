#include <unordered_map>
#include <CoreLib/make_unique.hpp>
#include <CoreLib/db.hpp>
#include <CoreLib/log.hpp>
#include "dbtables.hpp"
#include "rt.hpp"

using namespace std;
using namespace StockMarket;


struct DBTables::Impl
{
    std::unordered_map<std::string, std::string> TablesHash;
    std::unordered_map<std::string, std::string> FieldsHash;

    Impl();
    ~Impl();

    void InitHashes();
};

void DBTables::InitTables()
{

}

DBTables::DBTables() :
    m_pimpl(std::make_unique<DBTables::Impl>())
{
    m_pimpl->InitHashes();
}

DBTables::~DBTables()
{
}

string DBTables::Table(const std::string &id)
{
    if (m_pimpl->TablesHash.find(id) != m_pimpl->TablesHash.end()) {
        return m_pimpl->TablesHash[id];
    } else {
        LOG_ERROR("INVALID_TABLE_ID", id);
        return "{?}";
    }
}

string DBTables::Fields(const std::string &id)
{
    if (m_pimpl->FieldsHash.find(id) != m_pimpl->FieldsHash.end()) {
        return m_pimpl->FieldsHash[id];
    } else {
        LOG_ERROR("INVALID_FIELD_ID", id);
        return "{?}";
    }
}

void DBTables::SetFields(const std::string &tableId, const std::string &fields)
{
    m_pimpl->FieldsHash[tableId] = fields;
}

DBTables::Impl::Impl()
{

}

DBTables::Impl::~Impl()
{

}

void DBTables::Impl::InitHashes()
{
    TablesHash["LAST_UPDATE"] = "lastupdate";
    TablesHash["DATA_TITLES"] = "datatitles";
    TablesHash["STOCK_DATA"] = "stockdata";

    FieldsHash["LAST_UPDATE"] =
            " date TEXT NOT NULL, "
            " time TEXT NOT NULL ";

    FieldsHash["DATA_TITLES"] =
            " id TEXT NOT NULL, "
            " title TEXT NOT NULL, "
            " PRIMARY KEY ( id ) ";
}

