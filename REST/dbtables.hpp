#ifndef DBTABLES_HPP
#define DBTABLES_HPP


#include <string>

namespace StockMarket {
    class DBTables;
}

class StockMarket::DBTables
{
private:
    struct Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    static void InitTables();

public:
    DBTables();
    ~DBTables();

public:
    std::string Table(const std::string &id);
    std::string TableFromDate(const std::string &id, const std::string &date);
    std::string Fields(const std::string &id);

    void SetFields(const std::string &tableId, const std::string &fields);
};


#endif /* DBTABLES_HPP */

