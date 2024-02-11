
#include <filesystem>

#include <absl/strings/str_cat.h>
#include <ada.h>

#include <tempo_utils/internal/url_data.h>
#include <tempo_utils/unicode.h>
#include <tempo_utils/url.h>

tempo_utils::Url::Url()
{
}

tempo_utils::Url::Url(std::shared_ptr<internal::UrlData> priv)
    : m_priv(priv)
{
    TU_ASSERT (m_priv != nullptr);
}

tempo_utils::Url::Url(const Url &other)
    : m_priv(other.m_priv)
{
}

tempo_utils::Url::Url(Url &&other) noexcept
{
    m_priv = std::move(other.m_priv);
}

tempo_utils::Url&
tempo_utils::Url::operator=(const Url &other)
{
    if (this != &other) {
        m_priv = other.m_priv;
    }
    return *this;
}

tempo_utils::Url&
tempo_utils::Url::operator=(Url &&other) noexcept
{
    if (this != &other) {
        m_priv = std::move(other.m_priv);
    }
    return *this;
}

bool
tempo_utils::Url::isValid() const
{
    return m_priv != nullptr;
}

bool
tempo_utils::Url::isAbsolute() const
{
    return isValid() && hasScheme() && (hasAuthority() || hasPath());
}

bool
tempo_utils::Url::isAuthority() const
{
    return isValid() && hasAuthority();
}

bool
tempo_utils::Url::isRelative() const
{
    if (!isValid() || !hasPath())
        return false;
    return !hasScheme() && !hasOrigin() && !hasAuthority();
}

bool
tempo_utils::Url::isFragment() const
{
    if (!isValid() || !hasFragment())
        return false;
    return !hasScheme() && !hasOrigin() && !hasAuthority() && !hasPath() && !hasQuery();
}

bool
tempo_utils::Url::hasScheme() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->uri.get_protocol().empty();
}

bool
tempo_utils::Url::hasOrigin() const
{
    if (m_priv == nullptr)
        return false;
    const auto &uri = m_priv->uri;
    if (uri.type == ada::scheme::FILE)
        return !uri.get_protocol().empty();
    return !uri.get_protocol().empty() && !uri.get_hostname().empty();
}

bool
tempo_utils::Url::hasAuthority() const
{
    if (m_priv == nullptr)
        return false;
    const auto &uri = m_priv->uri;
    return uri.has_hostname() || uri.has_credentials();
}

bool
tempo_utils::Url::hasPath() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->uri.get_pathname().empty();
}

bool
tempo_utils::Url::hasQuery() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->uri.get_search().empty();
}

bool
tempo_utils::Url::hasFragment() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->uri.get_hash().empty();
}

bool
tempo_utils::Url::isKnownScheme() const
{
    return getKnownScheme() != KnownUrlScheme::Unknown;
}

tempo_utils::KnownUrlScheme
tempo_utils::Url::getKnownScheme() const
{
    switch (ada::scheme::get_scheme_type(schemeView())) {
        case ada::scheme::HTTP:
            return KnownUrlScheme::Http;
        case ada::scheme::HTTPS:
            return KnownUrlScheme::Https;
        case ada::scheme::FILE:
            return KnownUrlScheme::File;
        default:
            return KnownUrlScheme::Unknown;
    }
}

std::string
tempo_utils::Url::getScheme() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(schemeView());
}

std::string_view
tempo_utils::Url::schemeView() const
{
    if (m_priv == nullptr)
        return {};
    auto protocol = m_priv->uri.get_protocol();
    if (!protocol.empty() && protocol.back() == ':')
        return std::string_view(protocol.data(), protocol.size() - 1);
    return protocol;
}

std::string
tempo_utils::Url::getHost() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_hostname());
}

std::string_view
tempo_utils::Url::hostView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_hostname();
}

std::string
tempo_utils::Url::getPort() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_port());
}

std::string_view
tempo_utils::Url::portView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_port();
}

std::string
tempo_utils::Url::getHostAndPort() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_host());
}

std::string_view
tempo_utils::Url::hostAndPortView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_host();
}

std::string
tempo_utils::Url::getUsername() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_username());
}

std::string_view
tempo_utils::Url::usernameView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_username();
}

std::string
tempo_utils::Url::getPassword() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_password());
}

std::string_view
tempo_utils::Url::passwordView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_password();
}

std::string
tempo_utils::Url::getPath() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_pathname());
}

std::string_view
tempo_utils::Url::pathView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_pathname();
}

std::string
tempo_utils::Url::getQuery() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_search());
}

std::string_view
tempo_utils::Url::queryView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_search();
}

std::string
tempo_utils::Url::getFragment() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_hash());
}

std::string_view
tempo_utils::Url::fragmentView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->uri.get_hash();
}

std::string_view
tempo_utils::Url::uriView() const
{
    if (m_priv == nullptr)
        return std::string_view();
    return m_priv->uri.get_href();
}

tempo_utils::Url
tempo_utils::Url::traverse(const UrlPathPart &part)
{
    auto updatedPath = toPath().traverse(part);
    if (hasScheme() && hasAuthority()) {
        return fromAbsolute(schemeView(), toAuthority().toString(), updatedPath.pathView(),
            queryView(), fragmentView());
    } else if (hasAuthority()) {
        return fromAuthority(toAuthority(), updatedPath.pathView(), queryView(), fragmentView());
    } else {
        return fromRelative(updatedPath.pathView(), queryView(), fragmentView());
    }
}

tempo_utils::UrlOrigin
tempo_utils::Url::toOrigin() const
{
    if (hasOrigin())
        return UrlOrigin(m_priv);
    return {};
}

tempo_utils::UrlAuthority
tempo_utils::Url::toAuthority() const
{
    if (hasAuthority())
        return UrlAuthority(m_priv);
    return {};
}

tempo_utils::UrlPath
tempo_utils::Url::toPath() const
{
    if (hasPath())
        return UrlPath(m_priv);
    return {};
}

std::string
tempo_utils::Url::toString() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(m_priv->uri.get_href());
}

std::filesystem::path
tempo_utils::Url::toFilesystemPath(const std::filesystem::path &relativeBase) const
{
    if (schemeView() == "file")
        return std::filesystem::path(toPath().toString());
    return toPath().toFilesystemPath(relativeBase);
}

bool
tempo_utils::Url::validate(tu_uint16 validations) const
{
    if (m_priv == nullptr)
        return false;

    // if no validations are specified then only check for empty url (this is equivalent to isValid)
    if (validations == 0)
        return !m_priv->uri.get_href().empty();

    // otherwise perform each specified check
    if (validations & kHasScheme && m_priv->uri.get_protocol().empty())
        return false;
    if (validations & kHasUsername && m_priv->uri.get_username().empty())
        return false;
    if (validations & kHasPassword && m_priv->uri.get_password().empty())
        return false;
    if (validations & kHasHost && m_priv->uri.get_hostname().empty())
        return false;
    if (validations & kHasPort && m_priv->uri.get_port().empty())
        return false;
    if (validations & kHasPath && m_priv->uri.get_pathname().empty())
        return false;
    if (validations & kHasQuery && m_priv->uri.get_search().empty())
        return false;
    if (validations & kHasFragment && m_priv->uri.get_hash().empty())
        return false;

    return true;
}

bool
tempo_utils::Url::operator==(const tempo_utils::Url &other) const
{
    return m_priv.get() == other.m_priv.get()
        || uriView() == other.uriView();
}

bool
tempo_utils::Url::operator!=(const tempo_utils::Url &other) const
{
    return uriView() != other.uriView();
}

tempo_utils::Url
tempo_utils::Url::fromString(std::string_view s)
{
    ada::result<ada::url_aggregator> parseUrlResult = ada::parse<ada::url_aggregator>(s, nullptr);
    if (parseUrlResult)
        return Url(std::make_shared<internal::UrlData>(std::move(*parseUrlResult)));

    ada::url_aggregator base;
    ada::result<ada::url_aggregator> parseUrlWithBaseResult = ada::parse<ada::url_aggregator>(s, &base);
    if (parseUrlWithBaseResult)
        return Url(std::make_shared<internal::UrlData>(std::move(*parseUrlWithBaseResult)));

    return Url();
}

tempo_utils::Url
tempo_utils::Url::fromAbsolute(
    std::string_view scheme,
    UrlAuthority authority,
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    return fromAbsolute(scheme, authority.toString(), path, query, fragment);
}

tempo_utils::Url
tempo_utils::Url::fromAbsolute(
    std::string_view scheme,
    std::string_view authority,
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    if (scheme.empty() || authority.empty())
        return {};

    std::string s(scheme);
    s.append("://");
    s.append(authority);

    if (!path.empty()) {
        if (path.front() != '/') {
            s.append("/");
        }
        s.append(path);
    }
    if (!query.empty()) {
        if (query.front() != '?') {
            s.append("?");
        }
        s.append(query);
    }
    if (!fragment.empty()) {
        if (fragment.front() != '#') {
            s.append("#");
        }
        s.append(fragment);
    }
    return fromString(s);
}

tempo_utils::Url
tempo_utils::Url::fromAuthority(
    UrlAuthority authority,
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    if (authority.isEmpty())
        return {};
    return fromAuthority(authority.toString(), path, query, fragment);
}

tempo_utils::Url
tempo_utils::Url::fromAuthority(
    std::string_view authority,
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    if (authority.empty())
        return {};

    std::string s("//");
    s.append(authority);

    if (!path.empty()) {
        if (path.front() != '/') {
            s.append("/");
        }
        s.append(path);
    }
    if (!query.empty()) {
        if (query.front() != '?') {
            s.append("?");
        }
        s.append(query);
    }
    if (!fragment.empty()) {
        if (fragment.front() != '#') {
            s.append("#");
        }
        s.append(fragment);
    }
    return fromString(s);
}

tempo_utils::Url
tempo_utils::Url::fromOrigin(
    UrlOrigin origin,
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    if (origin.isEmpty())
        return {};
    return fromOrigin(origin.toString(), path, query, fragment);
}

tempo_utils::Url
tempo_utils::Url::fromOrigin(
    std::string_view origin,
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    if (origin.empty())
        return {};

    std::string s(origin);

    if (!path.empty()) {
        if (path.front() != '/') {
            s.append("/");
        }
        s.append(path);
    }
    if (!query.empty()) {
        if (query.front() != '?') {
            s.append("?");
        }
        s.append(query);
    }
    if (!fragment.empty()) {
        if (fragment.front() != '#') {
            s.append("#");
        }
        s.append(fragment);
    }
    return fromString(s);
}

tempo_utils::Url
tempo_utils::Url::fromRelative(
    std::string_view path,
    std::string_view query,
    std::string_view fragment)
{
    if (path.empty())
        return {};

    std::string s(path);

    if (!query.empty()) {
        if (query.front() != '?') {
            s.append("?");
        }
        s.append(query);
    }
    if (!fragment.empty()) {
        if (fragment.front() != '#') {
            s.append("#");
        }
        s.append(fragment);
    }
    return fromString(s);
}

tempo_utils::Url
tempo_utils::Url::fromFragment(std::string_view fragment)
{
    if (fragment.empty())
        return {};
    std::string s;
    if (fragment.front() != '#') {
        s.append("#");
    }
    s.append(fragment);
    return fromString(s);
}

tempo_utils::Url
tempo_utils::Url::fromFilesystemPath(const std::filesystem::path &path)
{
    std::error_code ec;
    auto absolutePath = absolute(path, ec);
    if (ec)
        return {};

    std::string s("file:///");

    if (path.has_root_name()) {
        s.append(path.root_name());
        s.append("/");
    }
    s.append(absolutePath.relative_path().generic_string());

    return fromString(s);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const tempo_utils::Url &uri)
{
    message.m_buffer << uri.toString();
    return std::move(message);
}
