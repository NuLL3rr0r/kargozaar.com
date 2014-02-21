#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
#include <vmime/vmime.hpp>

#if defined (_WIN32)
#include <vmime/platforms/windows/windowsHandler.hpp>
#else
#include <vmime/platforms/posix/posixHandler.hpp>
#endif  // defined (_WIN32)

#include "mail.hpp"
#include "make_unique.hpp"


#define     UNKNOWN_ERROR         "CoreLib::Mail unknown error!"


using namespace std;
using namespace boost;
using namespace CoreLib;

struct Mail::Impl
{
    std::string From;
    std::string To;
    std::string Subject;
    std::string Body;
    std::vector<std::string> Attachments;

    Impl();
    ~Impl();
};

Mail::Mail() :
    m_pimpl(std::make_unique<Mail::Impl>())
{
}

Mail::Mail(const std::string &from, const std::string &to,
           const std::string &subject, const std::string &body,
           const std::vector<std::string> &attachments) :
      m_pimpl(std::make_unique<Mail::Impl>())
{
    m_pimpl->From = from;
    m_pimpl->To = to;
    m_pimpl->Subject = subject;
    m_pimpl->Body = body;
    SetAttachments(attachments);
}

std::string Mail::GetFrom()
{
    return m_pimpl->From;
}

std::string Mail::GetTo()
{
    return m_pimpl->To;
}

std::string Mail::GetSubject()
{
    return m_pimpl->Subject;
}

std::string Mail::GetBody()
{
    return m_pimpl->Body;
}

const std::vector<std::string> &Mail::GetAttachments()
{
    return m_pimpl->Attachments;
}

void Mail::SetFrom(const std::string &from)
{
    m_pimpl->From = from;
}

void Mail::SetTo(const std::string &to)
{
    m_pimpl->To = to;
}

void Mail::SetSubject(const std::string &subject)
{
    m_pimpl->Subject = subject;
}

void Mail::SetBody(const std::string &body)
{
    m_pimpl->Body = body;
}

void Mail::SetAttachments(const std::vector<std::string> &attachments)
{
    m_pimpl->Attachments.clear();
    for (auto a : attachments) {
        m_pimpl->Attachments.push_back(a);
    }
}

bool Mail::Send() const
{
    string err;
    return Send(err);
}

bool Mail::Send(std::string &out_error) const
{
    try {
#if defined (_WIN32)
        vmime::platform::setHandler<vmime::platforms::windows::windowsHandler>();
#else
        vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();
#endif /* defined (_WIN32) */

        vmime::messageBuilder mb;

        mb.setExpeditor(vmime::mailbox(m_pimpl->From));
        mb.getRecipients().appendAddress(vmime::create<vmime::mailbox>(m_pimpl->To));

        mb.setSubject(*vmime::text::newFromString(m_pimpl->Subject, vmime::charsets::UTF_8));

        mb.constructTextPart(vmime::mediaType(vmime::mediaTypes::TEXT, vmime::mediaTypes::TEXT_HTML));
        mb.getTextPart()->setCharset(vmime::charsets::UTF_8);
        mb.getTextPart()->setText(vmime::create<vmime::stringContentHandler>(m_pimpl->Body));

        if (m_pimpl->Attachments.size() > 0) {
            for (auto a : m_pimpl->Attachments) {
                vmime::ref <vmime::attachment> att = vmime::create <vmime::fileAttachment>
                        (a, vmime::mediaType("application/octet-stream"),
                         vmime::text(boost::filesystem::path(a).stem().string()));
                mb.appendAttachment(att);
            }
        }

        vmime::ref<vmime::message> msg = mb.construct();

        vmime::utility::url url("smtp://localhost");
        vmime::ref<vmime::net::session> sess = vmime::create<vmime::net::session>();
        vmime::ref<vmime::net::transport> tr = sess->getTransport(url);

        tr->connect();
        tr->send(msg);
        tr->disconnect();

        return true;
    }

    catch (vmime::exception &ex) {
        out_error.assign(ex.what());
    }

    catch(std::exception &ex) {
        out_error.assign(ex.what());
    }

    catch (...) {
        out_error.assign(UNKNOWN_ERROR);
    }

    return false;
}

void Mail::SendAsync() const
{
    boost::thread t(static_cast<bool (Mail::*)() const>(&Mail::Send), this);
    t.detach();
}

Mail::Impl::Impl()
{

}

Mail::Impl::~Impl()
{

}

