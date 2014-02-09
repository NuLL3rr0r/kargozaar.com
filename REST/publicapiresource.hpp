#ifndef PUBLIC_API_RESOURCE_HPP
#define PUBLIC_API_RESOURCE_HPP


#include <Wt/WResource>

namespace Wt {
    namespace Http {
        class Request;
        class Response;
    }
}

namespace StockMarket {
    class PublicAPIResource;
}

class StockMarket::PublicAPIResource : public Wt::WResource
{
public:
    PublicAPIResource(WObject *parent = NULL);
    ~PublicAPIResource();

public:
    virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};


#endif /* PUBLIC_API_RESOURCE_HPP */

