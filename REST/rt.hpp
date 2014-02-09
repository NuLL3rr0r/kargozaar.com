#ifndef RT_HPP
#define RT_HPP


#include <memory>
#include <mutex>
#include "stockupdateworker.hpp"

namespace CoreLib {
    class DB;
}

namespace StockMarket {
    class DBTables;
    class RT;
}

class StockMarket::RT
{
public:
    struct StorageStruct
    {
        std::string AppPath;
    };

private:
    class Impl;
    static std::unique_ptr<Impl> s_pimpl;

public:
    static StorageStruct *Storage();
    static CoreLib::DB *DB();
    static StockMarket::DBTables *DBTables();
    static StockMarket::StockUpdateWorker *StockUpdateWorker();
};


#endif /* RT_HPP */

