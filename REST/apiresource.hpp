#ifndef API_RESOURCE_HPP
#define API_RESOURCE_HPP


#include <Wt/WResource>

namespace Wt {
    namespace Http {
        class Request;
        class Response;
    }
}

namespace StockMarket {
    class APIResource;
}

class StockMarket::APIResource : public Wt::WResource
{
private:
    struct Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    explicit APIResource(WObject *parent = NULL);
    virtual ~APIResource() = 0;

public:
    virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response) = 0;
};


#endif /* API_RESOURCE_HPP */

