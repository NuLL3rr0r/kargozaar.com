#include <boost/filesystem/path.hpp>
#include <CoreLib/db.hpp>
#include <CoreLib/make_unique.hpp>
#include <CoreLib/log.hpp>
#include "rt.hpp"
#include "dbtables.hpp"

using namespace std;
using namespace StockMarket;

struct RT::Impl
{
    typedef std::unique_ptr<StorageStruct> Storage_ptr;
    typedef std::unique_ptr<CoreLib::DB> DB_ptr;
    typedef std::unique_ptr<StockMarket::DBTables> DBTables_ptr;
    typedef std::unique_ptr<StockMarket::StockUpdateWorker> StockUpdateWorker_ptr;

    std::mutex StorageMutex;
    Storage_ptr StorageInstance;

    std::mutex DBMutex;
    DB_ptr DBInstance;

    std::mutex DBTablesMutex;
    DBTables_ptr DBTablesInstance;

    std::mutex StockUpdateWorkerMutex;
    StockUpdateWorker_ptr StockUpdateWorkerInstance;

    Impl();
    ~Impl();
};

std::unique_ptr<RT::Impl> RT::s_pimpl = std::make_unique<RT::Impl>();

RT::StorageStruct *RT::Storage()
{
    lock_guard<mutex> lock(s_pimpl->StorageMutex);
    (void)lock;

    if (s_pimpl->StorageInstance == nullptr) {
        s_pimpl->StorageInstance = std::make_unique<RT::StorageStruct>();
    }

    return s_pimpl->StorageInstance.get();
}

CoreLib::DB *RT::DB()
{
    lock_guard<mutex> lock(s_pimpl->DBMutex);
    (void)lock;

    if (s_pimpl->DBInstance == nullptr) {
        static const std::string DB_FILE((boost::filesystem::path(Storage()->AppPath)
                                          / boost::filesystem::path("..")
                                          / boost::filesystem::path("db")
                                          / boost::filesystem::path(STOCK_DATA_DB_FILE_NAME)).string());
        CoreLib::DB::Vacuum(DB_FILE);
#ifdef CORELIB_STATIC
        DB::LoadSQLite3Driver();
#endif  // CORELIB_STATIC
        s_pimpl->DBInstance = std::make_unique<CoreLib::DB>(DB_FILE);
    }

    return s_pimpl->DBInstance.get();
}

StockMarket::DBTables *RT::DBTables()
{
    lock_guard<mutex> lock(s_pimpl->DBTablesMutex);
    (void)lock;

    if (s_pimpl->DBTablesInstance == nullptr) {
        s_pimpl->DBTablesInstance = std::make_unique<StockMarket::DBTables>();
    }

    return s_pimpl->DBTablesInstance.get();
}

StockMarket::StockUpdateWorker *RT::StockUpdateWorker()
{
    lock_guard<mutex> lock(s_pimpl->StockUpdateWorkerMutex);
    (void)lock;

    if (s_pimpl->StockUpdateWorkerInstance == nullptr) {
        s_pimpl->StockUpdateWorkerInstance = std::make_unique<StockMarket::StockUpdateWorker>(
                    STOCK_DATA_SOURCE_URL,
                    STOCK_DATA_UPDATE_SECONDS_INTERVAL,
                    STOCK_DATA_LOCAL_TEMP_FILE,
                    true
                    );
    }

    return s_pimpl->StockUpdateWorkerInstance.get();
}


RT::Impl::Impl()
{

}

RT::Impl::~Impl()
{
    StorageInstance.reset();
    DBInstance.reset();
    StockUpdateWorkerInstance.reset();
}

