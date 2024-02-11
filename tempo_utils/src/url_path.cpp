
#include <ada.h>
#include <ada/url_aggregator-inl.h>

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
    : m_parts()
{
}

inline std::string_view::size_type
next_segment(std::string_view path, tu_int16 &index, std::pair<tu_int16,tu_int16> &segment)
{
    auto found = path.find('/', index);

    tu_int16 count;
    if (found == std::string_view::npos) {
        count = path.size() - index;
        segment = {index, count};
        index = path.size();
    } else {
        count = found - index;
        segment = {index, count};
        index = found + 1;
    }

    return found;
}

tempo_utils::UrlPath::UrlPath(std::shared_ptr<internal::UrlData> priv)
    : m_priv(priv)
{
    // special case: no url
    if (m_priv == nullptr)
        return;

    // special case: url path component is empty
    auto path = m_priv->uri.get_pathname();
    if (path.empty())
        return;

    // path length cannot exeed 2^15 because we use int16 for tracking offsets
    TU_ASSERT (path.size() < std::numeric_limits<tu_int16>::max());

    tu_int16 index = 0;
    std::pair<tu_int16,tu_int16> segment;
    std::string_view::size_type found;

    found = next_segment(path, index, segment);

    do {
        m_parts.push_back(segment);
        found = next_segment(path, index, segment);
    } while (!(segment.second == 0 && found == std::string_view::npos));
}

tempo_utils::UrlPath::UrlPath(
    std::shared_ptr<internal::UrlData> priv,
    std::vector<std::pair<tu_int16,tu_int16>> &&parts)
    : m_priv(priv),
      m_parts(std::move(parts))
{
}

tempo_utils::UrlPath::UrlPath(const UrlPath &other)
    : m_priv(other.m_priv),
      m_parts(other.m_parts)
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
    if (m_parts.empty())
        return true;
    if (m_parts.size() == 1) {
        const auto &part = m_parts.front();
        if (part.first == 0 && part.second == 0)
            return true;
    }
    return false;
}

int
tempo_utils::UrlPath::numParts() const
{
    return m_parts.empty()? 0 : m_parts.size() - 1;
}

tempo_utils::UrlPathPart
tempo_utils::UrlPath::getPart(int index) const
{
    return UrlPathPart(partView(index));
}

std::string_view
tempo_utils::UrlPath::partView(int index) const
{
    if (m_priv == nullptr)
        return {};
    // skip the absolute path marker
    index++;
    if (0 <= index && index < static_cast<int>(m_parts.size())) {
        auto &segment = m_parts[index];
        auto path = m_priv->uri.get_pathname();
        return path.substr(segment.first, segment.second);
    }
    return {};
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
    if (m_parts.empty())
        return {};
    return getPart(numParts() - 1);
}

std::string_view
tempo_utils::UrlPath::lastView() const
{
    if (m_parts.empty())
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
    return other.isAncestorOf(*this);
}

std::string_view
tempo_utils::UrlPath::pathView() const
{
    if (m_priv == nullptr)
        return {};
    auto view = m_priv->uri.get_pathname();
//    if (view.back() == '/')
//        return std::string_view(view.data(), view.size() - 1);
    return view;
}

tempo_utils::UrlPath
tempo_utils::UrlPath::traverse(const UrlPathPart &part)
{
    std::string appended(pathView());
    if (appended.back() != '/') {
        appended.append("/");
    }
    appended.append(part.partView());
    return fromString(appended);
}

std::string
tempo_utils::UrlPath::toString() const
{
    return std::string(pathView());
}

std::filesystem::path
tempo_utils::UrlPath::toFilesystemPath(const std::filesystem::path &relativeBase) const
{
    std::filesystem::path path(relativeBase);
    for (int i = 0; i < numParts(); i++) {
        path.append(partView(i));
    }
    return path;
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

tempo_utils::UrlPath
tempo_utils::UrlPath::fromString(std::string_view s)
{
    ada::result<ada::url_aggregator> parseUrlResult = ada::parse<ada::url_aggregator>(s, nullptr);
    if (parseUrlResult)
        return UrlPath(std::make_shared<internal::UrlData>(std::move(*parseUrlResult)));

    ada::url_aggregator base;
    ada::result<ada::url_aggregator> parseUrlWithBaseResult = ada::parse<ada::url_aggregator>(s, &base);
    if (parseUrlWithBaseResult)
        return UrlPath(std::make_shared<internal::UrlData>(std::move(*parseUrlWithBaseResult)));

    return UrlPath();
}
