#include <CoreLib/make_unique.hpp>
#include "apiresource.hpp"

using namespace std;
using namespace Wt;
using namespace StockMarket;

class APIResource::Impl
{
public:
    Impl();
    ~Impl();
};

APIResource::APIResource(WObject *parent) :
    WResource(parent),
    m_pimpl(std::make_unique<APIResource::Impl>())
{

}

APIResource::~APIResource()
{

}

APIResource::Impl::Impl()
{

}

APIResource::Impl::~Impl()
{

}

