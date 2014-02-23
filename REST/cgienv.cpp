#include <boost/lexical_cast.hpp>
#include <Wt/WEnvironment>
#include <GeoIP.h>
#include <GeoIPCity.h>
#include <CoreLib/io.hpp>
#include <CoreLib/make_unique.hpp>
#include "cgienv.hpp"

using namespace std;
using namespace Wt;
using namespace boost;
using namespace CoreLib;
using namespace StockMarket;

struct CgiEnv::Impl
{
    std::string ClientInfoIP;
    std::string ClientInfoBrowser;
    std::string ClientInfoReferer;
    std::string ClientInfoLocation;

    std::string ServerInfoHost;
    std::string ServerInfoURL;
    std::string ServerInfoNoReplyAddr;

    bool FoundXSS;

    std::string InitialURL;

private:
    CgiEnv *m_cgiEnv;

public:

    static std::string CStrToStr(const char *cstr);

    Impl(CgiEnv *cgiEnv);
    ~Impl();

    void ExtractClientInfoDetail();
};

CgiEnv::CgiEnv(const WEnvironment &env) :
    m_pimpl(std::make_unique<CgiEnv::Impl>(this))
{
    m_pimpl->ClientInfoIP = env.getCgiValue("REMOTE_ADDR");
    m_pimpl->ClientInfoBrowser = env.getCgiValue("HTTP_USER_AGENT");
    m_pimpl->ClientInfoReferer = env.getCgiValue("HTTP_REFERER");

    m_pimpl->ExtractClientInfoDetail();

    m_pimpl->ClientInfoLocation = ClientInfoRecord.city + "; "
            + ClientInfoRecord.region + "; "
            + ClientInfoRecord.country_code + "; "
            + ClientInfoRecord.country_code3 + "; "
            + ClientInfoRecord.country_name + "; "
            + ClientInfoRecord.continent_code;

    m_pimpl->ServerInfoHost = env.getCgiValue("HTTP_HOST");
    m_pimpl->ServerInfoURL = "http://" + m_pimpl->ServerInfoHost + env.getCgiValue("REQUEST_URI");
    m_pimpl->ServerInfoNoReplyAddr = "no-reply@" + m_pimpl->ServerInfoHost;

    string queryStr = env.getCgiValue("QUERY_STRING");
    m_pimpl->FoundXSS = (queryStr.find("<") != string::npos ||
            queryStr.find(">") != string::npos ||
            queryStr.find("%3C") != string::npos ||
            queryStr.find("%3E") != string::npos ||
            queryStr.find("%3c") != string::npos ||
            queryStr.find("%3e") != string::npos)
            ? true : false;
}

CgiEnv::~CgiEnv()
{

}

bool CgiEnv::FoundXSS()
{
    return m_pimpl->FoundXSS;
}

string CgiEnv::Impl::CStrToStr(const char *cstr)
{
    return cstr != NULL ? cstr : "";
}

CgiEnv::Impl::Impl(CgiEnv *cgiEnv) :
    m_cgiEnv(cgiEnv)
{

}

CgiEnv::Impl::~Impl()
{

}

void CgiEnv::Impl::ExtractClientInfoDetail()
{
    try {
        GeoIP *geoLiteCity;
        if (IO::FileExists("/usr/local/share/GeoIP/GeoLiteCity.dat")) {
            geoLiteCity = GeoIP_open("/usr/local/share/GeoIP/GeoLiteCity.dat", GEOIP_STANDARD);
        } else if (IO::FileExists("/usr/share/GeoIP/GeoLiteCity.dat")) {
            geoLiteCity = GeoIP_open("/usr/share/GeoIP/GeoLiteCity.dat", GEOIP_STANDARD);
        } else {
            return;
        }

        GeoIPRecordTag *record = GeoIP_record_by_name(geoLiteCity, ClientInfoIP.c_str());

        if (record != NULL) {
            m_cgiEnv->ClientInfoRecord.country_code = CStrToStr(record->country_code);
            m_cgiEnv->ClientInfoRecord.country_code3 = CStrToStr(record->country_code3);
            m_cgiEnv->ClientInfoRecord.country_name = CStrToStr(record->country_name);
            m_cgiEnv->ClientInfoRecord.region = CStrToStr(record->region);
            m_cgiEnv->ClientInfoRecord.city = CStrToStr(record->city);
            m_cgiEnv->ClientInfoRecord.postal_code = CStrToStr(record->postal_code);
            m_cgiEnv->ClientInfoRecord.latitude = lexical_cast<string>(record->latitude);
            m_cgiEnv->ClientInfoRecord.longitude = lexical_cast<string>(record->longitude);
            m_cgiEnv->ClientInfoRecord.metro_code = lexical_cast<string>(record->metro_code);
            m_cgiEnv->ClientInfoRecord.dma_code = lexical_cast<string>(record->dma_code);
            m_cgiEnv->ClientInfoRecord.area_code = lexical_cast<string>(record->area_code);
            m_cgiEnv->ClientInfoRecord.charset = lexical_cast<string>(record->charset);
            m_cgiEnv->ClientInfoRecord.continent_code = CStrToStr(record->continent_code);
            m_cgiEnv->ClientInfoRecord.country_conf = lexical_cast<string>(record->country_conf);
            m_cgiEnv->ClientInfoRecord.region_conf = lexical_cast<string>(record->region_conf);
            m_cgiEnv->ClientInfoRecord.city_conf = lexical_cast<string>(record->city_conf);
            m_cgiEnv->ClientInfoRecord.postal_conf = lexical_cast<string>(record->postal_conf);
            m_cgiEnv->ClientInfoRecord.accuracy_radius = lexical_cast<string>(record->accuracy_radius);
        }
    } catch(...) {
    }
}

