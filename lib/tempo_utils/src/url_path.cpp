
#include <tempo_utils/internal/url_data.h>
#include <tempo_utils/log_message.h>
#include <tempo_utils/unicode.h>
#include <tempo_utils/url_path.h>

tempo_utils::UrlPathPart::UrlPathPart()
{
}

tempo_utils::UrlPathPart::UrlPathPart(const char *part)
    : m_part(percent_encode_utf8(part))
{
}

tempo_utils::UrlPathPart::UrlPathPart(std::string_view part)
    : m_part(percent_encode_utf8(part))
{
}

tempo_utils::UrlPathPart::UrlPathPart(const UrlPathPart &other)
    : m_part(other.m_part)
{
}

bool
tempo_utils::UrlPathPart::isValid() const
{
    return !m_part.empty();
}

std::string
tempo_utils::UrlPathPart::getPart() const
{
    return m_part;
}

std::string_view
tempo_utils::UrlPathPart::partView() const
{
    return m_part;
}

std::string
tempo_utils::UrlPathPart::decode() const
{
    return percent_decode_utf8(m_part);
}

tempo_utils::UrlPathPart
tempo_utils::UrlPathPart::curr()
{
    return UrlPathPart(".");
}

tempo_utils::UrlPathPart
tempo_utils::UrlPathPart::parent()
{
    return UrlPathPart("..");
}

tempo_utils::UrlPath::UrlPath()
{
}

tempo_utils::UrlPath::UrlPath(std::shared_ptr<internal::UrlData> priv)
    : m_priv(priv)
{
}

tempo_utils::UrlPath::UrlPath(const UrlPath &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_utils::UrlPath::isValid() const
{
    return m_priv != nullptr;
}

bool
tempo_utils::UrlPath::isEmpty() const
{
    if (!isValid())
        return true;
    return m_priv->url.encoded_segments().empty();
}

bool
tempo_utils::UrlPath::isAbsolute() const
{
    if (!isValid())
        return false;
    return m_priv->url.encoded_segments().is_absolute();
}

bool
tempo_utils::UrlPath::isRelative() const
{
    if (!isValid())
        return false;
    return !m_priv->url.encoded_segments().is_absolute();
}

int
tempo_utils::UrlPath::numParts() const
{
    if (!isValid())
        return 0;
    return m_priv->url.encoded_segments().size();
}

tempo_utils::UrlPathPart
tempo_utils::UrlPath::getPart(int index) const
{
    return UrlPathPart(partView(index));
}

/**
 * Returns the percent-encoded path segment at the specified index. The index can be positive or negative; in
 * the negative case the indexing starts from the end of the path.
 * @param index The index.
 * @return A string view containing the percent-encoded path part, or an empty string view if the index was invalid.
 */
std::string_view
tempo_utils::UrlPath::partView(int index) const
{
    if (!isValid())
        return {};
    if (numParts() <= index || index < 0 - numParts())
        return {};
    auto iterator = m_priv->url.encoded_segments().begin();
    std::advance(iterator, index);
    return *iterator;
}

tempo_utils::UrlPathPart
tempo_utils::UrlPath::getHead() const
{
    return getPart(0);
}

std::string_view
tempo_utils::UrlPath::headView() const
{
    return partView(0);
}

tempo_utils::UrlPathPart
tempo_utils::UrlPath::getLast() const
{
    if (!isValid())
        return {};
    return getPart(numParts() - 1);
}

std::string_view
tempo_utils::UrlPath::lastView() const
{
    if (!isValid())
        return {};
    return partView(numParts() - 1);
}

tempo_utils::UrlPath
tempo_utils::UrlPath::getInit() const
{
    if (m_priv == nullptr)
        return {};
    if (isEmpty())
        return *this;
    if (numParts() == 1)
        return fromString("/");
    std::string init;
    for (int i = 0; i < numParts() - 1; i++) {
        init.append("/").append(partView(i));
    }
    return fromString(init);
}

tempo_utils::UrlPath
tempo_utils::UrlPath::getTail() const
{
    if (m_priv == nullptr)
        return {};
    if (isEmpty())
        return *this;
    if (numParts() == 1)
        return fromString("/");
    std::string tail;
    for (int i = 1; i < numParts(); i++) {
        tail.append("/").append(partView(i));
    }
    return fromString(tail);
}

/**
 * Returns true if path is the parent (i.e. direct ancestor) of the other path, otherwise false.
 *
 * @param other The path to compare against.
 * @return true if path is the parent, otherwise false.
 */
bool
tempo_utils::UrlPath::isParentOf(const UrlPath &other) const
{
    if (numParts() != other.numParts() - 1)
        return false;
    for (int i = 0; i < numParts(); i++) {
        if (partView(i) != other.partView(i))
            return false;
    }
    return true;
}

/**
 * Returns true if path is the child (i.e. direct descendent) of the other path, otherwise false.
 *
 * @param other The path to compare against.
 * @return true if path is the child, otherwise false.
 */
bool
tempo_utils::UrlPath::isChildOf(const UrlPath &other) const
{
    if (m_priv == nullptr)
        return false;
    return other.isParentOf(*this);
}

/**
 * Returns true if path is the ancestor of the other path, otherwise false.
 *
 * @param other The path to compare against.
 * @return true if path is the ancestor, otherwise false.
 */
bool
tempo_utils::UrlPath::isAncestorOf(const UrlPath &other) const
{
    if (other.numParts() <= numParts())
        return false;
    for (int i = 0; i < numParts(); i++) {
        if (partView(i) != other.partView(i))
            return false;
    }
    return true;
}

/**
 * Returns true if path is the descendent of the other path, otherwise false.
 *
 * @param other The path to compare against.
 * @return true if path is the descendent, otherwise false.
 */
bool
tempo_utils::UrlPath::isDescendentOf(const UrlPath &other) const
{
    if (m_priv == nullptr)
        return false;
    return other.isAncestorOf(*this);
}

std::string_view
tempo_utils::UrlPath::pathView() const
{
    if (m_priv == nullptr)
        return {};
    return m_priv->url.encoded_path();
}

tempo_utils::UrlPath
tempo_utils::UrlPath::traversePart(const UrlPathPart &part) const
{
    std::string appended(pathView());
    if (appended.back() != '/') {
        appended.append("/");
    }
    appended.append(part.partView());

    if (appended.size() > 1024)
        return {};
    boost::urls::static_url<1024> url(appended);
    if (url.empty())
        return {};

    url.normalize();

    return fromString(url.buffer());
}

tempo_utils::UrlPath
tempo_utils::UrlPath::traversePath(const UrlPath &path) const
{
    if (path.isAbsolute())
        return path;
    auto traversed = *this;
    for (int i = 0; i < path.numParts(); i++) {
        traversed = traversed.traverse(path.getPart(i));
    }
    return traversed;
}

std::string
tempo_utils::UrlPath::toString() const
{
    if (m_priv == nullptr)
        return {};
    return std::string(pathView());
}

std::filesystem::path
tempo_utils::UrlPath::toFilesystemPath(const std::filesystem::path &relativeBase) const
{
    std::filesystem::path path(relativeBase);
    for (int i = 0; i < numParts(); i++) {
        path.append(partView(i));
    }
    return path.lexically_normal();
}

bool
tempo_utils::UrlPath::operator==(const UrlPath &other) const
{
    return pathView() == other.pathView();
}

bool
tempo_utils::UrlPath::operator!=(const UrlPath &other) const
{
    return pathView() != other.pathView();
}

/**
 * Construct a new UrlPath from the specified string. Note that this function will successfully parse a full URI
 * or URI reference as well as a path.
 *
 * @param s The string to parse.
 * @return A new UrlPath containing the path, or an empty UrlPath if the input string was invalid.
 */
tempo_utils::UrlPath
tempo_utils::UrlPath::fromString(std::string_view s)
{
    auto priv = std::make_shared<internal::UrlData>();
    priv->data = s;
    auto parseUrlResult = boost::urls::parse_uri_reference(priv->data);
    if (parseUrlResult.has_error())
        return {};
    priv->url = *parseUrlResult;
    return UrlPath(priv);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const tempo_utils::UrlPath &path)
{
    message.m_buffer << path.toString();
    return std::move(message);
}
