#include <vector>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/regex.hpp>
#include <CoreLib/archiver.hpp>
#include <CoreLib/db.hpp>
#include <CoreLib/http.hpp>
#include <CoreLib/io.hpp>
#include <CoreLib/log.hpp>
#include <CoreLib/make_unique.hpp>
#include "stockupdateworker.hpp"
#include "dbtables.hpp"
#include "rt.hpp"

using namespace std;
using namespace boost;
using namespace CoreLib;
using namespace StockMarket;

struct StockUpdateWorker::Impl
{
    typedef std::unique_ptr<boost::thread> thread_ptr;

    bool Running;
    bool StartImmediately;

    std::string SourceURL;
    Interval_t Interval;
    std::string TempFile;

    thread_ptr WorkerThread;
    std::mutex WorkerMutex;

    Impl();
    ~Impl();

    void Cron();
    void Update();
};

StockUpdateWorker::StockUpdateWorker() :
    m_pimpl(std::make_unique<StockUpdateWorker::Impl>())
{

}

StockUpdateWorker::StockUpdateWorker(const std::string &sourceURL,
                                     const StockUpdateWorker::Interval_t interval,
                                     const std::string &tempFile,
                                     bool startImmediately) :
    m_pimpl(std::make_unique<StockUpdateWorker::Impl>())
{
    m_pimpl->SourceURL = sourceURL;
    m_pimpl->Interval = interval;
    m_pimpl->TempFile = tempFile;
    m_pimpl->StartImmediately = startImmediately;
}

StockUpdateWorker::~StockUpdateWorker()
{

}

std::string StockUpdateWorker::GetSourceURL()
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    return m_pimpl->SourceURL;
}

boost::int_least64_t StockUpdateWorker::GetInterval()
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    return m_pimpl->Interval;
}

std::string StockUpdateWorker::GetTempFile()
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    return m_pimpl->TempFile;
}

void StockUpdateWorker::SetSourceURL(const std::string &sourceFile)
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    if (m_pimpl->Running)
        return;

   m_pimpl->SourceURL = sourceFile;
}

void StockUpdateWorker::SetInterval(boost::int_least64_t interval)
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    if (m_pimpl->Running)
        return;

    m_pimpl->Interval = interval;
}

void StockUpdateWorker::SetTempFile(const std::string &tempFile)
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    if (m_pimpl->Running)
        return;

    m_pimpl->TempFile = tempFile;
}

bool StockUpdateWorker::IsRunning()
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    return m_pimpl->Running;
}

void StockUpdateWorker::Start()
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    assert(m_pimpl->SourceURL != "");
    assert(m_pimpl->Interval != 0);
    assert(m_pimpl->TempFile != "");

    if (m_pimpl->Running) {
        return;
    }

    LOG_INFO("Starting stock data updater process...");

    m_pimpl->Running = true;

    m_pimpl->WorkerThread = std::make_unique<thread>(&StockUpdateWorker::Impl::Cron, m_pimpl.get());
    m_pimpl->WorkerThread->detach();
}

void StockUpdateWorker::Stop()
{
    std::lock_guard<std::mutex> lock(m_pimpl->WorkerMutex);
    (void)lock;

    if (!m_pimpl->Running) {
        return;
    }

    LOG_INFO("Stopping stock data updater process...");

    m_pimpl->Running = false;
    m_pimpl->WorkerThread->interrupt();

    m_pimpl->WorkerThread.reset();
}

StockUpdateWorker::Impl::Impl() :
    Running(false),
    StartImmediately(false)
{

}

StockUpdateWorker::Impl::~Impl()
{
    std::lock_guard<std::mutex> lock(WorkerMutex);
    (void)lock;

    WorkerThread->interrupt();
    WorkerThread.reset();
}

void StockUpdateWorker::Impl::Cron()
{
    if (!StartImmediately) {
        boost::this_thread::interruption_point();
        boost::this_thread::sleep_for(boost::chrono::seconds(STOCK_DATA_UPDATE_SECONDS_INTERVAL));
    }

    for (;;) {
        boost::this_thread::disable_interruption di;

        {
            std::lock_guard<std::mutex> lock(WorkerMutex);
            (void)lock;

            Update();

            if (!Running)
                break;
        }

        boost::this_thread::restore_interruption ri(di);
        (void)ri;
        boost::this_thread::interruption_point();
        boost::this_thread::sleep_for(boost::chrono::seconds(STOCK_DATA_UPDATE_SECONDS_INTERVAL));
    }
}

void StockUpdateWorker::Impl::Update()
{
    static const std::string TEMP_FILE(
                (boost::filesystem::path(RT::Storage()->AppPath)
                 / boost::filesystem::path("..")
                 / boost::filesystem::path("tmp")
                 / STOCK_DATA_LOCAL_TEMP_FILE).string()
            );
    static const std::string WORK_DIR(
                (boost::filesystem::path(RT::Storage()->AppPath)
                 / boost::filesystem::path("..")
                 / boost::filesystem::path("tmp")
                 / STOCK_DATA_TEMP_WORK_DIR).string()
            );

    string err;

    if (IO::DirExists(WORK_DIR))
        IO::EraseDir(WORK_DIR);

    if (IO::FileExists(TEMP_FILE))
        IO::EraseFile(TEMP_FILE);

    if (HTTP::DownloadFile(SourceURL, TEMP_FILE, err)) {
        if (Archiver::UnZip(TEMP_FILE, WORK_DIR, err)) {

            try {
                static const std::string SHARED_STRINGS_FILE(
                            (boost::filesystem::path(WORK_DIR)
                             / boost::filesystem::path("xl")
                             / boost::filesystem::path("sharedStrings.xml")).string()
                             );
                static const std::string SHEET1_FILE(
                            (boost::filesystem::path(WORK_DIR)
                             / boost::filesystem::path("xl")
                             / boost::filesystem::path("worksheets")
                             / boost::filesystem::path("sheet1.xml")).string()
                             );

                property_tree::ptree sharedStringsTree;
                property_tree::read_xml(SHARED_STRINGS_FILE, sharedStringsTree);

                vector<std::string> sharedStrings;

                BOOST_FOREACH(property_tree::ptree::value_type &siNode,
                              sharedStringsTree.get_child("sst")) {
                    auto t = siNode.second.get_child_optional("t");
                    if (t) {
                        sharedStrings.push_back(siNode.second.get<std::string>("t"));
                    }
                }

                property_tree::ptree sheet1Tree;
                property_tree::read_xml(SHEET1_FILE, sheet1Tree);

                string date;
                string time;
                string createTableFields = " r INTEGER NOT NULL, ";
                vector<string> tableFieldsId;

                cppdb::transaction guard(RT::DB()->SQL());

                BOOST_FOREACH(property_tree::ptree::value_type &rowNode,
                              sheet1Tree.get_child("worksheet.sheetData")) {

                    long rRow(rowNode.second.get<long>("<xmlattr>.r", -1));

                    if (rRow == 2) {
                        RT::DB()->DropTable(RT::DBTables()->Table("DATA_TITLES"));
                        RT::DB()->CreateTable(RT::DBTables()->Table("DATA_TITLES"),
                                              RT::DBTables()->Fields("DATA_TITLES"));
                    } else if (rRow == 3) {
                        createTableFields += " PRIMARY KEY ( r ) ";

                        /// We should set this each and every time
                        /// due to any possible changes in original
                        /// .xlsx file
                        RT::DBTables()->SetFields("STOCK_DATA",
                                                  createTableFields);

                        RT::DB()->DropTable(RT::DBTables()->Table("STOCK_DATA"));
                        RT::DB()->CreateTable(RT::DBTables()->Table("STOCK_DATA"),
                                              RT::DBTables()->Fields("STOCK_DATA"));
                    }

                    if (rRow > 2) {
                        RT::DB()->Insert(RT::DBTables()->Table("STOCK_DATA"),
                                         "r", 1,
                                         boost::lexical_cast<string>(rRow).c_str());
                    }

                    size_t col = 0;

                    BOOST_FOREACH(property_tree::ptree::value_type &cNode,
                                  rowNode.second) {
                        if (cNode.first == "c") {
                            if (rRow == 1) {
                                string rC(cNode.second.get<std::string>("<xmlattr>.r", ""));
                                string tC(cNode.second.get<std::string>("<xmlattr>.t", ""));

                                if (rC == "A1" && tC == "s") {
                                    std::string v(sharedStrings[cNode.second.get<size_t>("v")]);

                                    static const regex eDate("(13)[1-9][1-9][\\/]((0[1-9]|1[012])|([1-9]))[\\/]((0[1-9]|[12][0-9]|3[01])|([1-9]))");
                                    static const regex eTime("[0-2][1-9][\\:](([0-9][0-9])|([0-9]))[\\:](([0-9][0-9])|([0-9]))");

                                    boost::smatch result;
                                    if (boost::regex_search(v, result, eDate)) {
                                        date.assign(result[0].first, result[0].second);
                                        if (date.size() != 10 || date.size() != 0) {
                                            std::vector<std::string> vec;
                                            boost::split(vec, date, boost::is_any_of(L"/"));
                                            if (vec.size() == 3) {
                                                date = (boost::format("%1%/%2%/%3%")
                                                    % vec[0]
                                                    % (vec[1].size() == 2 ? vec[1] : (boost::format("0%1%") % vec[1]).str())
                                                    % (vec[2].size() == 2 ? vec[2] : (boost::format("0%1%") % vec[2]).str())
                                                    ).str();
                                            }
                                        }
                                    }
                                    if (boost::regex_search(v, result, eTime)) {
                                        time.assign(result[0].first, result[0].second);
                                        if (time.size() != 10 || time.size() != 0) {
                                            std::vector<std::string> vec;
                                            boost::split(vec, time, boost::is_any_of(L":"));
                                            if (vec.size() == 3) {
                                                time = (boost::format("%1%:%2%:%3%")
                                                    % vec[0]
                                                    % (vec[1].size() == 2 ? vec[1] : (boost::format("0%1%") % vec[1]).str())
                                                    % (vec[2].size() == 2 ? vec[2] : (boost::format("0%1%") % vec[2]).str())
                                                    ).str();
                                            }
                                        }
                                    }

                                    try {
                                        cppdb::result r = RT::DB()->SQL()
                                                << (boost::format("SELECT date, time"
                                                                  " FROM %1%"
                                                                  " ORDER BY ROWID ASC"
                                                                  " LIMIT 1;")
                                                    % RT::DBTables()->Table("LAST_UPDATE")).str()
                                                   << cppdb::row;

                                        if (!r.empty()) {
                                            string lastUpdateDate;
                                            string lastUpdateTime;
                                            r >> lastUpdateDate >> lastUpdateTime;

                                            if (lastUpdateDate == date && lastUpdateTime == time) {
                                                guard.rollback();
                                                return;
                                            } else {
                                                if (lastUpdateDate != date) {
                                                    std::string archiveDataTitlesTableName(
                                                                RT::DBTables()->TableFromDate("DATA_TITLES", lastUpdateDate));
                                                    std::string archiveStockDataTableName(
                                                                RT::DBTables()->TableFromDate("STOCK_DATA", lastUpdateDate));

                                                    RT::DB()->RenameTable(
                                                                RT::DBTables()->Table("STOCK_DATA"),
                                                                archiveStockDataTableName
                                                                );
                                                    RT::DB()->RenameTable(
                                                                RT::DBTables()->Table("DATA_TITLES"),
                                                                archiveDataTitlesTableName
                                                                );

                                                    RT::DB()->Insert(RT::DBTables()->Table("ARCHIVE"),
                                                                     "date, time, datatitlestbl, stockdatatbl ", 4,
                                                                     lastUpdateDate.c_str(),
                                                                     lastUpdateTime.c_str(),
                                                                     archiveDataTitlesTableName.c_str(),
                                                                     archiveStockDataTableName.c_str()
                                                                     );
                                                }
                                            }
                                        }
                                    } catch (...) {
                                    }

                                    break;
                                }
                            } else if (rRow == 2) {
                                string tC(cNode.second.get<std::string>("<xmlattr>.t", ""));
                                if (tC == "s") {
                                    string rC(cNode.second.get<std::string>("<xmlattr>.r", ""));
                                    string v(sharedStrings[cNode.second.get<size_t>("v")]);

                                    RT::DB()->Insert(RT::DBTables()->Table("DATA_TITLES"),
                                                     "id, title", 2,
                                                     rC.c_str(),
                                                     v.c_str());

                                    tableFieldsId.push_back(rC);
                                    createTableFields += (boost::format(" [%1%] TEXT, ") % rC).str();
                                }
                            } else {
                                string tC(cNode.second.get<std::string>("<xmlattr>.t", ""));
                                string v(tC == "s"
                                         ? sharedStrings[cNode.second.get<size_t>("v")]
                                         : boost::lexical_cast<string>(cNode.second.get<double>("v")));
                                RT::DB()->Update(RT::DBTables()->Table("STOCK_DATA"),
                                                 "r", boost::lexical_cast<string>(rRow),
                                                 (boost::format("%1%=?") % tableFieldsId[col]).str(), 1,
                                                 v.c_str());
                            }
                            ++col;
                        }
                    }
                }

                RT::DB()->DropTable(RT::DBTables()->Table("LAST_UPDATE"));
                RT::DB()->CreateTable(RT::DBTables()->Table("LAST_UPDATE"),
                                      RT::DBTables()->Fields("LAST_UPDATE"));
                RT::DB()->Insert(RT::DBTables()->Table("LAST_UPDATE"),
                                 "date, time",
                                 2,
                                 date.c_str(), time.c_str());

                guard.commit();
            }

            catch (boost::exception &ex) {
                LOG_ERROR(boost::diagnostic_information(ex));
            }

            catch (std::exception &ex) {
                LOG_ERROR(ex.what());
            }

            catch (...) {
                LOG_ERROR("StockUpdateWorker::Impl::Update(): Unknown error!");
            }

        } else {
            LOG_ERROR(err);
        }
    } else {
        LOG_ERROR(err);
    }

    if (IO::DirExists(WORK_DIR))
        IO::EraseDir(WORK_DIR);

    if (IO::FileExists(TEMP_FILE))
        IO::EraseFile(TEMP_FILE);
}

