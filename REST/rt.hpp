#ifndef RT_HPP
#define RT_HPP


#include <memory>
#include <mutex>
#include "stockupdateworker.hpp"

namespace CoreLib {
    class Crypto;
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
    struct Impl;
    static std::unique_ptr<Impl> s_pimpl;

public:
    static StorageStruct *Storage();
    static CoreLib::DB *DB();
    static StockMarket::DBTables *DBTables();
    static StockMarket::StockUpdateWorker *StockUpdateWorker();
    static CoreLib::Crypto *TokenClient();
    static CoreLib::Crypto *TokenServer();
};


#endif /* RT_HPP */

