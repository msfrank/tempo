
#include <tempo_utils/internal/url_data.h>
#include <tempo_utils/log_message.h>
#include <tempo_utils/url_authority.h>

tempo_utils::UrlAuthority::UrlAuthority()
{
}

tempo_utils::UrlAuthority::UrlAuthority(std::shared_ptr<internal::UrlData> priv)
    : m_priv(priv)
{
    TU_ASSERT (m_priv != nullptr);
}

tempo_utils::UrlAuthority::UrlAuthority(const UrlAuthority &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_utils::UrlAuthority::isValid() const
{
    return m_priv != nullptr;
}

bool
tempo_utils::UrlAuthority::isEmpty() const
{
    if (m_priv == nullptr)
        return true;
    return !hasCredentials() && !hasHost() && !hasPort();
}

bool
tempo_utils::UrlAuthority::hasUsername() const
{
    if (m_priv == nullptr)
        return {};
    return !m_priv->url.encoded_user().empty();
}

bool
tempo_utils::UrlAuthority::hasPassword() const
{
    if (m_priv == nullptr)
        return {};
    return !m_priv->url.encoded_password().empty();
}

bool
tempo_utils::UrlAuthority::hasCredentials() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.has_userinfo();
}

bool
tempo_utils::UrlAuthority::hasHost() const
{
    if (m_priv == nullptr)
        return {};
    return !m_priv->url.encoded_host().empty();
}

bool
tempo_utils::UrlAuthority::hasPort() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.has_port();
}

std::string
tempo_utils::UrlAuthority::getUsername() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::make_pct_string_view(usernameView())->decode();
}

std::string_view
tempo_utils::UrlAuthority::usernameView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_user();
}

std::string
tempo_utils::UrlAuthority::getPassword() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(passwordView()).decode();
}

std::string_view
tempo_utils::UrlAuthority::passwordView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_password();
}

std::string
tempo_utils::UrlAuthority::getHost() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.host();
}

std::string_view
tempo_utils::UrlAuthority::hostView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_host();
}

std::string
tempo_utils::UrlAuthority::getPort() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.port();
}

std::string_view
tempo_utils::UrlAuthority::portView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.port();
}

std::string
tempo_utils::UrlAuthority::getHostAndPort() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(hostAndPortView()).decode();
}

std::string_view
tempo_utils::UrlAuthority::hostAndPortView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_host_and_port();
}

std::string_view
tempo_utils::UrlAuthority::authorityView() const
{
    return m_priv->url.authority().buffer();
}

std::string
tempo_utils::UrlAuthority::toString() const
{
    return boost::urls::pct_string_view(authorityView()).decode();
}

bool
tempo_utils::UrlAuthority::operator==(const UrlAuthority &other) const
{
    if (m_priv.get() == other.m_priv.get())
        return true;
    if (m_priv == nullptr)
        return other.m_priv == nullptr;
    if (other.m_priv == nullptr)
        return false;
    return boost::urls::authority_view(m_priv->url) == boost::urls::authority_view(other.m_priv->url);
}

bool
tempo_utils::UrlAuthority::operator!=(const UrlAuthority &other) const
{
    return !(*this == other);
}
