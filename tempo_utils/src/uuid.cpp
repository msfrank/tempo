
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <tempo_utils/log_stream.h>
#include <tempo_utils/uuid.h>

struct tempo_utils::UUID::Priv {
    boost::uuids::uuid uuid;
    Priv(boost::uuids::uuid &&uuid): uuid(std::move(uuid)) {};
};

tempo_utils::UUID::UUID()
    : m_priv()
{
}

tempo_utils::UUID::UUID(std::shared_ptr<Priv> priv)
    : m_priv(std::move(priv))
{
    TU_ASSERT (m_priv != nullptr);
}

bool
tempo_utils::UUID::isValid()
{
    return m_priv != nullptr;
}

std::string
tempo_utils::UUID::toString() const
{
    if (m_priv == nullptr)
        return {};
    return boost::uuids::to_string(m_priv->uuid);
}

bool
tempo_utils::UUID::operator==(const UUID &other) const
{
    if (m_priv == nullptr && other.m_priv == nullptr)
        return true;
    if (m_priv != nullptr && other.m_priv != nullptr)
        return m_priv->uuid == other.m_priv->uuid;
    return false;
}

bool
tempo_utils::UUID::operator!=(const UUID &other) const
{
    return !(*this == other);
}

void
tempo_utils::UUID::hashValue(absl::HashState state) const
{
    if (m_priv != nullptr) {
        absl::HashState::combine(std::move(state), m_priv->uuid.data);
    } else {
        absl::HashState::combine(std::move(state), 0);
    }
}

tempo_utils::UUID
tempo_utils::UUID::randomUUID()
{
    boost::uuids::random_generator uuidgen;
    auto priv = std::make_shared<UUID::Priv>(uuidgen());
    return UUID(priv);
}
