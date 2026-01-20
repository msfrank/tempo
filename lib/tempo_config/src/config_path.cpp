
#include <tempo_config/config_path.h>

#include "tempo_config/config_types.h"

tempo_config::ConfigPathPart::ConfigPathPart()
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ConfigPathPartType::Invalid;
}

tempo_config::ConfigPathPart::ConfigPathPart(std::shared_ptr<Priv> priv)
    : m_priv(priv)
{
}

tempo_config::ConfigPathPart::ConfigPathPart(int index)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ConfigPathPartType::Index;
    m_priv->index = index;
}

tempo_config::ConfigPathPart::ConfigPathPart(std::string_view key)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ConfigPathPartType::Key;
    m_priv->key = key;
}

tempo_config::ConfigPathPart::ConfigPathPart(const ConfigPathPart &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_config::ConfigPathPart::isValid() const
{
    return m_priv->type != ConfigPathPartType::Invalid;
}

tempo_config::ConfigPathPartType
tempo_config::ConfigPathPart::getType() const
{
    return m_priv->type;
}

int
tempo_config::ConfigPathPart::getIndex() const
{
    if (m_priv->type == ConfigPathPartType::Index)
        return m_priv->index;
    return -1;
}

std::string
tempo_config::ConfigPathPart::getKey() const
{
    if (m_priv->type == ConfigPathPartType::Key)
        return m_priv->key;
    return {};
}

tempo_config::ConfigPath::ConfigPath()
    : m_priv(std::make_shared<Priv>())
{
}

tempo_config::ConfigPath::ConfigPath(std::vector<ConfigPathPart> &&parts)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->parts = std::move(parts);
}

tempo_config::ConfigPath::ConfigPath(std::shared_ptr<Priv> priv)
    : m_priv(std::move(priv))
{
}

tempo_config::ConfigPath::ConfigPath(const ConfigPath &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_config::ConfigPath::isRoot() const
{
    return m_priv->parts.empty();
}

std::vector<tempo_config::ConfigPathPart>::const_iterator
tempo_config::ConfigPath::partsBegin() const
{
    return m_priv->parts.cbegin();
}

std::vector<tempo_config::ConfigPathPart>::const_iterator
tempo_config::ConfigPath::partsEnd() const
{
    return m_priv->parts.cend();
}

int
tempo_config::ConfigPath::numParts() const
{
    return m_priv->parts.size();
}

tempo_config::ConfigPath
tempo_config::ConfigPath::traverse(const ConfigPathPart &part) const
{
    if (part.getType() == ConfigPathPartType::Invalid)
        return {};
    auto priv = std::make_shared<Priv>();
    priv->parts.insert(priv->parts.cbegin(), m_priv->parts.cbegin(), m_priv->parts.cend());
    priv->parts.push_back(part);
    return ConfigPath(priv);
}

tempo_config::ConfigPath
tempo_config::ConfigPath::parent() const
{
    if (isRoot())
        return *this;
    std::vector parts(m_priv->parts.cbegin(), m_priv->parts.cend() - 1);
    return ConfigPath(std::move(parts));
}