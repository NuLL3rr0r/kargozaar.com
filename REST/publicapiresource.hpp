#ifndef PUBLIC_API_RESOURCE_HPP
#define PUBLIC_API_RESOURCE_HPP


#include "apiresource.hpp"

namespace Wt {
    namespace Http {
        class Request;
        class Response;
    }
}

namespace StockMarket {
    class PublicAPIResource;
}

class StockMarket::PublicAPIResource : public StockMarket::APIResource
{
public:
    explicit PublicAPIResource(WObject *parent = NULL);
    virtual ~PublicAPIResource();

public:
    virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};


#endif /* PUBLIC_API_RESOURCE_HPP */

