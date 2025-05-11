
#include <filesystem>

#include <absl/strings/str_cat.h>

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
    return m_priv->url.has_scheme();
}

bool
tempo_utils::Url::hasOrigin() const
{
    if (m_priv == nullptr)
        return false;
    const auto &url = m_priv->url;
    // an origin must have a scheme
    if (!url.has_scheme())
        return false;
    // as a special case a file scheme is considered to have an origin even if there is no authority
    if (url.scheme_id() == boost::urls::scheme::file)
        return true;
    return url.has_authority();
}

bool
tempo_utils::Url::hasAuthority() const
{
    if (m_priv == nullptr)
        return false;
    return m_priv->url.has_authority();
}

bool
tempo_utils::Url::hasPath() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->url.encoded_path().empty();
}

bool
tempo_utils::Url::hasQuery() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->url.encoded_query().empty();
}

bool
tempo_utils::Url::hasFragment() const
{
    if (m_priv == nullptr)
        return false;
    return !m_priv->url.encoded_fragment().empty();
}

bool
tempo_utils::Url::isKnownScheme() const
{
    return getKnownScheme() != KnownUrlScheme::Unknown;
}

tempo_utils::KnownUrlScheme
tempo_utils::Url::getKnownScheme() const
{
    if (m_priv == nullptr)
        return KnownUrlScheme::Unknown;
    switch (m_priv->url.scheme_id()) {
        case boost::urls::scheme::none:
            return KnownUrlScheme::NoScheme;
        case boost::urls::scheme::http:
            return KnownUrlScheme::Http;
        case boost::urls::scheme::https:
            return KnownUrlScheme::Https;
        case boost::urls::scheme::file:
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
    return m_priv->url.scheme();
}

std::string
tempo_utils::Url::getHost() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(hostView()).decode();
}

std::string_view
tempo_utils::Url::hostView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_host();
}

std::string
tempo_utils::Url::getPort() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.port();
}

std::string_view
tempo_utils::Url::portView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.port();
}

std::string_view
tempo_utils::Url::hostAndPortView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_host_and_port();
}

std::string
tempo_utils::Url::getHostAndPort() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(hostAndPortView()).decode();
}

std::string
tempo_utils::Url::getUsername() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(usernameView()).decode();
}

std::string_view
tempo_utils::Url::usernameView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_user();
}

std::string
tempo_utils::Url::getPassword() const
{
    if (m_priv == nullptr)
        return {};
    return boost::urls::pct_string_view(passwordView()).decode();
}

std::string_view
tempo_utils::Url::passwordView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_password();
}

std::string
tempo_utils::Url::getPath() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.path();
}

std::string_view
tempo_utils::Url::pathView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_path();
}

std::string
tempo_utils::Url::getQuery() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.query();
}

std::string_view
tempo_utils::Url::queryView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_query();
}

std::string
tempo_utils::Url::getFragment() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.fragment();
}

std::string_view
tempo_utils::Url::fragmentView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_fragment();
}

std::string_view
tempo_utils::Url::uriView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->data;
}

tempo_utils::Url
tempo_utils::Url::traversePart(const UrlPathPart &part) const
{
    auto updatedPath = toPath().traverse(part);
    if (hasScheme() && hasAuthority())
        return fromAbsolute(schemeView(), toAuthority().toString(), updatedPath.pathView(),
            queryView(), fragmentView());

    if (hasAuthority())
        return fromAuthority(toAuthority(), updatedPath.pathView(), queryView(), fragmentView());

    return fromRelative(updatedPath.pathView(), queryView(), fragmentView());
}

tempo_utils::Url
tempo_utils::Url::traversePath(const UrlPath &path) const
{
    auto updatedPath = toPath().traversePath(path);
    if (hasScheme() && hasAuthority())
        return fromAbsolute(schemeView(), toAuthority().toString(), updatedPath.pathView(),
            queryView(), fragmentView());

    if (hasAuthority())
        return fromAuthority(toAuthority(), updatedPath.pathView(), queryView(), fragmentView());

    return fromRelative(updatedPath.pathView(), queryView(), fragmentView());
}

tempo_utils::Url
tempo_utils::Url::resolve(const Url &other) const
{
    if (!isAbsolute())
        return {};
    if (!other.isValid())
        return *this;
    boost::url dest;
    auto result = boost::urls::resolve(m_priv->url, other.m_priv->url, dest);
    if (result.has_error())
        return {};
    return fromString(dest.c_str());
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
    return boost::urls::pct_string_view(m_priv->data).decode();
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
        return !m_priv->url.empty();

    // otherwise perform each specified check
    if (validations & kHasScheme && !hasScheme())
        return false;
    if (validations & kHasUsername && usernameView().empty())
        return false;
    if (validations & kHasPassword && passwordView().empty())
        return false;
    if (validations & kHasHost && hostView().empty())
        return false;
    if (validations & kHasPort && portView().empty())
        return false;
    if (validations & kHasPath && pathView().empty())
        return false;
    if (validations & kHasQuery && queryView().empty())
        return false;
    if (validations & kHasFragment && fragmentView().empty())
        return false;

    return true;
}

bool
tempo_utils::Url::operator==(const tempo_utils::Url &other) const
{
    if (m_priv.get() == other.m_priv.get())
        return true;
    if (m_priv == nullptr)
        return other.m_priv == nullptr;
    if (other.m_priv == nullptr)
        return false;
    return m_priv->url == other.m_priv->url;
}

bool
tempo_utils::Url::operator!=(const tempo_utils::Url &other) const
{
    return uriView() != other.uriView();
}

tempo_utils::Url
tempo_utils::Url::fromString(std::string_view s)
{
    if (s.empty())
        return {};
    auto priv = std::make_shared<internal::UrlData>();
    priv->data = s;
    auto parseUrlResult = boost::urls::parse_uri_reference(priv->data);
    if (parseUrlResult.has_error())
        return {};
    priv->url = *parseUrlResult;
    return Url(priv);
}

tempo_utils::Url
tempo_utils::Url::fromAbsolute(
    std::string_view scheme,
    const UrlAuthority &authority,
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
    const UrlAuthority &authority,
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
    const UrlOrigin &origin,
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

tempo_utils::Url
tempo_utils::Url::withScheme(std::string_view scheme) const
{
    if (m_priv == nullptr)
        return {};
    boost::url updated(m_priv->url);
    if (scheme.empty()) {
        updated.remove_scheme();
    } else {
        updated.set_scheme(scheme);
    }
    return fromString(updated.buffer());
}

tempo_utils::Url
tempo_utils::Url::withPath(std::string_view path) const
{
    if (m_priv == nullptr)
        return {};
    boost::url updated(m_priv->url);
    if (path.empty()) {
        updated.set_path("");
        updated.set_path_absolute(false);
    } else {
        updated.set_path(path);
        bool isAbsolute = path.starts_with('/');
        updated.set_path_absolute(isAbsolute);
    }
    return fromString(updated.buffer());
}

tempo_utils::Url
tempo_utils::Url::withQuery(std::string_view query) const
{
    if (m_priv == nullptr)
        return {};
    boost::url updated(m_priv->url);
    if (query.empty()) {
        updated.remove_query();
    } else {
        updated.set_query(query);
    }
    return fromString(updated.buffer());
}

tempo_utils::Url
tempo_utils::Url::withFragment(std::string_view fragment) const
{
    if (m_priv == nullptr)
        return {};
    boost::url updated(m_priv->url);
    if (fragment.empty()) {
        updated.remove_fragment();
    } else {
        updated.set_fragment(fragment);
    }
    return fromString(updated.buffer());
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const tempo_utils::Url &uri)
{
    message.m_buffer << uri.toString();
    return std::move(message);
}
