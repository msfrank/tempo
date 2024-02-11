
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
    return m_priv->uri.has_non_empty_username();
}

bool
tempo_utils::UrlAuthority::hasPassword() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.has_password();
}

bool
tempo_utils::UrlAuthority::hasCredentials() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.has_credentials();
}

bool
tempo_utils::UrlAuthority::hasHost() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.has_hostname();
}

bool
tempo_utils::UrlAuthority::hasPort() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.has_port();
}

std::string
tempo_utils::UrlAuthority::getUsername() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_username());
}

std::string_view
tempo_utils::UrlAuthority::usernameView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_username();
}

std::string
tempo_utils::UrlAuthority::getPassword() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_password());
}

std::string_view
tempo_utils::UrlAuthority::passwordView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_password();
}

std::string
tempo_utils::UrlAuthority::getHost() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_hostname());
}

std::string_view
tempo_utils::UrlAuthority::hostView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_hostname();
}

std::string
tempo_utils::UrlAuthority::getPort() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_port());
}

std::string_view
tempo_utils::UrlAuthority::portView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_port();
}

std::string
tempo_utils::UrlAuthority::getHostAndPort() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_host());
}

std::string_view
tempo_utils::UrlAuthority::hostAndPortView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_host();
}

std::string
tempo_utils::UrlAuthority::toString() const
{
    std::string authority;
    if (hasCredentials()) {
        authority.append(usernameView());
        if (hasPassword()) {
            authority.push_back(':');
            authority.append(passwordView());
        }
        authority.push_back('@');
    }
    return authority.append(hostAndPortView());
}

bool
tempo_utils::UrlAuthority::operator==(const UrlAuthority &other) const
{
    if (m_priv == nullptr)
        return other.m_priv == nullptr;
    if (other.m_priv == nullptr)
        return false;
    return usernameView() == other.usernameView()
           && passwordView() == other.passwordView()
           && hostAndPortView() == other.hostAndPortView();
}

bool
tempo_utils::UrlAuthority::operator!=(const UrlAuthority &other) const
{
    return !(*this == other);
}
