
#include <tempo_config/internal/json_object.h>

tempo_config::internal::JsonObject::JsonObject(const ConfigLocation &location)
    : m_location(location)
{
}

tempo_config::internal::JsonObject::~JsonObject()
{
    for (auto &member : m_members) {
        delete member.second;
    }
}

tempo_config::internal::JsonNodeType
tempo_config::internal::JsonObject::getNodeType() const
{
    return JsonNodeType::Object;
}

tempo_config::ConfigNode
tempo_config::internal::JsonObject::getConfig() const
{
    absl::flat_hash_map<std::string,tempo_config::ConfigNode> map;
    for (const auto &member : m_members) {
        map[member.first] = member.second->getConfig();
    }
    return tempo_config::ConfigMap(map, m_location);
}

bool
tempo_config::internal::JsonObject::hasMember(std::string_view key) const
{
    return m_members.contains(key);
}

tempo_config::internal::AbstractJsonNode *
tempo_config::internal::JsonObject::getMember(std::string_view key) const
{
    if (m_members.contains(key))
        return m_members.at(key);
    return nullptr;
}

void
tempo_config::internal::JsonObject::putMember(std::string_view key, AbstractJsonNode *value)
{
    if (m_members.contains(key)) {
        delete m_members.at(key);
    }
    m_members[key] = value;
}

absl::flat_hash_map<std::string,tempo_config::internal::AbstractJsonNode *>::const_iterator
tempo_config::internal::JsonObject::cbegin() const
{
    return m_members.cbegin();
}

absl::flat_hash_map<std::string,tempo_config::internal::AbstractJsonNode *>::const_iterator
tempo_config::internal::JsonObject::cend() const
{
    return m_members.cend();
}

int
tempo_config::internal::JsonObject::numMembers() const
{
    return m_members.size();
}