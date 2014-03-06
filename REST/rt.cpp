#include <boost/filesystem/path.hpp>
#include <CoreLib/crypto.hpp>
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
    typedef std::unique_ptr<CoreLib::Crypto> Crypto_ptr;
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

    std::mutex TokenClientMutex;
    Crypto_ptr TokenClientInstance;

    std::mutex TokenServerMutex;
    Crypto_ptr TokenServerInstance;

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

CoreLib::Crypto *RT::TokenClient()
{
    lock_guard<mutex> lock(s_pimpl->TokenClientMutex);
    (void)lock;

    if (s_pimpl->TokenClientInstance == nullptr) {
        // Use this nice HEX/ASCII converter and your editor's replace dialog,
        // to create your own Key and IV.
        // http://www.dolcevie.com/js/converter.html

        // `7mDbC7kKy1/.?#$
        static constexpr CoreLib::Crypto::Byte_t KEY[] = {
            0x60, 0x37, 0x6d, 0x44, 0x62, 0x43, 0x37, 0x6b, 0x4b, 0x79, 0x31, 0x2f, 0x2e, 0x3f, 0x23, 0x24
        };

        // `imVbC7nMt1/.?@@
        static constexpr CoreLib::Crypto::Byte_t IV[] = {
            0x60, 0x69, 0x6d, 0x56, 0x62, 0x43, 0x37, 0x6e, 0x4d, 0x74, 0x31, 0x2f, 0x2e, 0x3f, 0x40, 0x40
        };

        s_pimpl->TokenClientInstance = std::make_unique<CoreLib::Crypto>(KEY, sizeof(KEY), IV, sizeof(IV));
    }

    return s_pimpl->TokenClientInstance.get();
}

CoreLib::Crypto *RT::TokenServer()
{
    lock_guard<mutex> lock(s_pimpl->TokenServerMutex);
    (void)lock;

    if (s_pimpl->TokenServerInstance == nullptr) {
        // Use this nice HEX/ASCII converter and your editor's replace dialog,
        // to create your own Key and IV.
        // http://www.dolcevie.com/js/converter.html

        // `7mbS7wkKy3/.?er
        static constexpr CoreLib::Crypto::Byte_t KEY[] = {
            0x60, 0x37, 0x6d, 0x62, 0x53, 0x37, 0x77, 0x6b, 0x4b, 0x79, 0x33, 0x2f, 0x2e, 0x3f, 0x65, 0x72
        };

        // `imVS7wkoP3/.?3r
        static constexpr CoreLib::Crypto::Byte_t IV[] = {
            0x60, 0x69, 0x6d, 0x56, 0x53, 0x37, 0x77, 0x6b, 0x6f, 0x50, 0x33, 0x2f, 0x2e, 0x3f, 0x33, 0x72
        };

        s_pimpl->TokenServerInstance = std::make_unique<CoreLib::Crypto>(KEY, sizeof(KEY), IV, sizeof(IV));
    }

    return s_pimpl->TokenServerInstance.get();
}

RT::Impl::Impl()
{

}

RT::Impl::~Impl()
{
    StorageInstance.reset();
    DBInstance.reset();
    StockUpdateWorkerInstance.reset();
    TokenClientInstance.reset();
    TokenServerInstance.reset();
}

