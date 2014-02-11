#ifndef STOCK_UPDATE_WORKER_HPP
#define STOCK_UPDATE_WORKER_HPP


#include <memory>
#include <mutex>
#include <string>
#include <boost/thread/thread.hpp>

namespace StockMarket {
    class StockUpdateWorker;
}

class StockMarket::StockUpdateWorker
{
public:
    typedef boost::int_least64_t Interval_t;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    StockUpdateWorker();
    StockUpdateWorker(const std::string &sourceURL,
                      const StockUpdateWorker::Interval_t interval,
                      const std::string &tempFile,
                      bool startImmediately = false);
    ~StockUpdateWorker();

public:
    std::string GetSourceURL();
    boost::int_least64_t GetInterval();
    std::string GetTempFile();

    void SetSourceURL(const std::string &sourceFile);
    void SetInterval(boost::int_least64_t interval);
    void SetTempFile(const std::string &tempFile);

public:
    bool IsRunning();
    void Start();
    void Stop();
};


#endif /* STOCK_UPDATE_WORKER_HPP */

