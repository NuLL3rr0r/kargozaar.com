#ifndef CGIENV_HPP
#define CGIENV_HPP


#include <string>

namespace Wt {
    class WEnvironment;
}

namespace StockMarket {
    class CgiEnv;
}

class StockMarket::CgiEnv
{
public:
    struct ClientInfoTag {
        std::string country_code;
        std::string country_code3;
        std::string country_name;
        std::string region;
        std::string city;
        std::string postal_code;
        std::string latitude;
        std::string longitude;
        std::string metro_code;
        std::string dma_code;
        std::string area_code;
        std::string charset;
        std::string continent_code;
        std::string country_conf;
        std::string region_conf;
        std::string city_conf;
        std::string postal_conf;
        std::string accuracy_radius;
    } ClientInfoRecord;

    enum class ClientInfo {
        IP,
        Browser,
        Referer,
        Location
    };

    enum ServerInfo {
        Host,
        URL,
        NoReplyAddr
    };

private:
    struct Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    CgiEnv(const Wt::WEnvironment &env);
    ~CgiEnv();

public:
    std::string GetClientInfo(const ClientInfo &key) const;
    std::string GetServerInfo(const ServerInfo &key) const;

    bool FoundXSS();
};


#endif /* CGIENV_HPP */

