#ifndef MAILER_HPP
#define MAILER_HPP


#include <string>
#include <vector>

namespace CoreLib {
    class Mail;
}

class CoreLib::Mail
{
private:
    struct Impl;
    std::unique_ptr<Impl> m_pimpl;

public:
    Mail();
    Mail(const std::string &from, const std::string &to,
         const std::string &subject, const std::string &body,
         const std::vector<std::string> &attachments = {  });

public:
    std::string GetFrom();
    std::string GetTo();
    std::string GetSubject();
    std::string GetBody();
    const std::vector<std::string> &GetAttachments();
    void SetFrom(const std::string &from);
    void SetTo(const std::string &to);
    void SetSubject(const std::string &subject);
    void SetBody(const std::string &body);
    void SetAttachments(const std::vector<std::string> &attachments);

public:
    bool Send() const;
    bool Send(std::string &out_error) const;
    void SendAsync() const;
};


#endif /* MAILER_HPP */


