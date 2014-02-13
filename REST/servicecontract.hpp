#ifndef SERVICE_CONTRACT_HPP
#define SERVICE_CONTRACT_HPP


#include <map>
#include <utility>
#include <vector>
#include <cstddef>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <CoreLib/make_unique.hpp>
#include "servicecontract.hpp"

namespace StockMarket {
    template<typename TARGET_T> class ServiceContract;
}

template<typename TARGET_T>
class StockMarket::ServiceContract
{
private:
    typedef std::multimap<std::wstring,
    std::pair<std::wstring, std::size_t> >
    Contracts_t;

    typedef void(TARGET_T::*FunctionPtr_t)();

private:
    Contracts_t m_contracts;
    TARGET_T *m_target;

public:
    explicit ServiceContract(TARGET_T *target) :
        m_target(target)
    {
    }

public:
    void Register(const std::wstring &uriTemplate)
    {
        std::vector<std::wstring> vec;
        boost::split(vec, uriTemplate, boost::is_any_of(L"/"));

        std::size_t argsCount = 0;
        for (std::vector<std::wstring>::const_iterator it = vec.begin();
             it != vec.end(); ++it) {
            if (boost::starts_with((*it), "{")
                    && boost::ends_with((*it), "}")) {
                ++argsCount;
            }
        }
        static const boost::wregex eOmmiter(L"\\/{.*?\\}");
        static const std::wstring emptyText = L"";
        std::wstring webMethod = boost::regex_replace(uriTemplate, eOmmiter, emptyText);

        m_contracts.insert(
                    std::make_pair(webMethod,
                                   std::make_pair(uriTemplate, argsCount)));
    }

    bool Resolve(const std::wstring &uri, std::wstring &out_uriTemplate, std::vector<std::wstring> &out_args) const
    {
        out_uriTemplate.clear();

        if (!boost::ends_with(uri, "/")) {
            for (Contracts_t::const_iterator it = m_contracts.begin();
                 it != m_contracts.end(); ++it) {
                if (boost::starts_with(uri, it->first)) {
                    std::wstring args(uri.substr(it->first.size()));

                    std::size_t argsCount = 0;
                    std::vector<std::wstring> vec;
                    if (args.find(L"/") != std::wstring::npos) {
                        boost::split(vec, args, boost::is_any_of(L"/"));

                        for (std::vector<std::wstring>::const_iterator vecIt = vec.begin();
                             vecIt != vec.end(); ++vecIt) {
                            if (!(*vecIt).empty()) {
                                out_args.push_back(*vecIt);
                                ++argsCount;
                            }
                        }
                    }

                    if (it->second.second == argsCount) {
                        out_uriTemplate = it->second.first;
                        return true;
                    }
                }
            }
        }

        return false;
    }
};


#endif /* SERVICE_CONTRACT_HPP */

