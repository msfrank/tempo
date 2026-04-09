
#include <absl/strings/escaping.h>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <tempo_utils/log_stream.h>
#include <tempo_utils/uuid.h>

struct tempo_utils::UUID::Priv {
    boost::uuids::uuid uuid;
    Priv(boost::uuids::uuid uuid): uuid(uuid) {};
};

tempo_utils::UUID::UUID()
{
}

tempo_utils::UUID::UUID(std::shared_ptr<Priv> priv)
    : m_priv(std::move(priv))
{
    TU_NOTNULL (m_priv);
}

bool
tempo_utils::UUID::isValid() const
{
    return m_priv != nullptr;
}

bool
tempo_utils::UUID::isNil() const
{
    return m_priv != nullptr && m_priv->uuid.is_nil();
}

std::vector<tu_uint8>
tempo_utils::UUID::toBytes() const
{
    if (m_priv == nullptr)
        return {};
    return std::vector<tu_uint8>(m_priv->uuid.begin(), m_priv->uuid.end());
}

std::span<const tu_uint8>
tempo_utils::UUID::bytesView() const
{
    if (m_priv == nullptr)
        return {};
    return std::span(m_priv->uuid.data(), m_priv->uuid.size());
}

std::string
tempo_utils::UUID::toRfc4122String() const
{
    if (m_priv == nullptr)
        return {};
    std::string_view sv((const char *) m_priv->uuid.data(), m_priv->uuid.size());

    auto block1 = absl::BytesToHexString(sv.substr(0, 4));
    auto block2 = absl::BytesToHexString(sv.substr(4, 2));
    auto block3 = absl::BytesToHexString(sv.substr(6, 2));
    auto block4 = absl::BytesToHexString(sv.substr(8, 2));
    auto block5 = absl::BytesToHexString(sv.substr(10, 6));

    return absl::StrCat(block1,
        "-", block2,
        "-", block3,
        "-", block4,
        "-", block5);
}

std::string
tempo_utils::UUID::toCompactString() const
{
    if (m_priv == nullptr)
        return {};
    std::string_view sv((const char *) m_priv->uuid.data(), m_priv->uuid.size());
    return absl::BytesToHexString(sv);
}

std::string
tempo_utils::UUID::toEnclosedString() const
{
    return absl::StrCat("{", toRfc4122String(), "}");
}

std::string
tempo_utils::UUID::toEnclosedCompactString() const
{
    return absl::StrCat("{", toCompactString(), "}");
}

int
tempo_utils::UUID::compare(const UUID &other) const
{
    if (m_priv) {
        if (other.m_priv == nullptr)
            return 1;
        auto cmp = memcmp(m_priv->uuid.data(), other.m_priv->uuid.data(), boost::uuids::uuid::static_size());
        if (cmp == 0)
            return 0;
        return cmp < 0? -1 : 1;
    }
    return other.m_priv? -1 : 0;
}

bool
tempo_utils::UUID::operator==(const UUID &other) const
{
    return compare(other) == 0;
}

bool
tempo_utils::UUID::operator!=(const UUID &other) const
{
    return compare(other) != 0;
}

bool
tempo_utils::UUID::operator<(const UUID &other) const
{
    return compare(other) < 0;
}

void
tempo_utils::UUID::hashValue(absl::HashState state) const
{
    if (m_priv != nullptr) {
        absl::HashState::combine(std::move(state), m_priv->uuid);
    } else {
        absl::HashState::combine(std::move(state), 0);
    }
}

tempo_utils::UUID
tempo_utils::UUID::nilUUID()
{
    auto priv = std::make_shared<Priv>(boost::uuids::nil_uuid());
    return UUID(priv);
}

tempo_utils::UUID
tempo_utils::UUID::randomUUID()
{
    boost::uuids::random_generator uuidgen;
    auto priv = std::make_shared<Priv>(uuidgen());
    return UUID(priv);
}

tempo_utils::UUID
tempo_utils::UUID::parse(std::string_view s)
{
    boost::uuids::nil_uuid();
    boost::uuids::string_generator gen;
    try {
        auto uuid = gen(s.begin(), s.end());
        auto priv = std::make_shared<Priv>(uuid);
        return UUID(priv);
    } catch (std::exception &ex) {
        return {};
    }
}
