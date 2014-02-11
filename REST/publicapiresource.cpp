#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <CoreLib/log.hpp>
#include "publicapiresource.hpp"

using namespace Wt;
using namespace StockMarket;

PublicAPIResource::PublicAPIResource(WObject *parent) :
    APIResource(parent)
{

}

PublicAPIResource::~PublicAPIResource()
{
    beingDeleted();
}

void PublicAPIResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{

}


