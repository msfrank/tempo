
#include <tempo_utils/internal/url_data.h>
#include <tempo_utils/log_message.h>
#include <tempo_utils/url_origin.h>

tempo_utils::UrlOrigin::UrlOrigin()
{
}

tempo_utils::UrlOrigin::UrlOrigin(std::shared_ptr<internal::UrlData> priv)
    : m_priv(priv)
{
    TU_ASSERT (m_priv != nullptr);
}

tempo_utils::UrlOrigin::UrlOrigin(const UrlOrigin &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_utils::UrlOrigin::isValid() const
{
    return m_priv != nullptr;
}

bool
tempo_utils::UrlOrigin::isEmpty() const
{
    if (m_priv == nullptr)
        return true;
    return !hasScheme() && !hasHost() && !hasPort();
}

bool
tempo_utils::UrlOrigin::hasScheme() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.has_scheme();
}

bool
tempo_utils::UrlOrigin::hasHost() const
{
    if (m_priv == nullptr)
        return {};
    return !m_priv->url.encoded_host().empty();
}

bool
tempo_utils::UrlOrigin::hasPort() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.has_port();
}

std::string
tempo_utils::UrlOrigin::getScheme() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.scheme();
}

std::string_view
tempo_utils::UrlOrigin::schemeView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.scheme();
}

std::string
tempo_utils::UrlOrigin::getHost() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(hostView()).decode();
}

std::string_view
tempo_utils::UrlOrigin::hostView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_host();
}

std::string
tempo_utils::UrlOrigin::getPort() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.port();
}

std::string_view
tempo_utils::UrlOrigin::portView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.port();
}

std::string
tempo_utils::UrlOrigin::getHostAndPort() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(hostAndPortView()).decode();
}

std::string_view
tempo_utils::UrlOrigin::hostAndPortView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_host_and_port();
}

std::string
tempo_utils::UrlOrigin::toString() const
{
    if (hasHost()) {
        std::string origin;
        if (hasScheme()) {
            origin.append(schemeView());
            origin.append(":");
        }
        origin.append("//");
        origin.append(hostView());
        if (hasPort()) {
            origin.push_back(':');
            origin.append(portView());
        }
        return boost::urls::pct_string_view(origin).decode();
    }
    return {};
}

bool
tempo_utils::UrlOrigin::operator==(const UrlOrigin &other) const
{
    if (m_priv.get() == other.m_priv.get())
        return true;
    if (m_priv == nullptr)
        return other.m_priv == nullptr;
    if (other.m_priv == nullptr)
        return false;
    return schemeView() == other.schemeView() && hostAndPortView() == other.hostAndPortView();
}

bool
tempo_utils::UrlOrigin::operator!=(const UrlOrigin &other) const
{
    return !(*this == other);
}

tempo_utils::UrlOrigin
tempo_utils::UrlOrigin::fromString(std::string_view s)
{
    if (s.empty())
        return {};
    auto priv = std::make_shared<internal::UrlData>();
    priv->data = s;
    auto parseUrlResult = boost::urls::parse_uri_reference(priv->data);
    if (parseUrlResult.has_error())
        return {};
    priv->url = *parseUrlResult;
    return UrlOrigin(priv);
}