
#include <tempo_config/config_path.h>

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

tempo_config::ConfigPathPartType
tempo_config::ConfigPathPart::getType() const
{
    return m_priv->type;
}

int
tempo_config::ConfigPathPart::getIndex()
{
    if (m_priv->type == ConfigPathPartType::Index)
        return m_priv->index;
    return -1;
}

std::string
tempo_config::ConfigPathPart::getKey()
{
    if (m_priv->type == ConfigPathPartType::Key)
        return m_priv->key;
    return {};
}

tempo_config::ConfigPathPart
tempo_config::ConfigPathPart::root()
{
    auto priv = std::make_shared<Priv>();
    priv->type = ConfigPathPartType::Root;
    return ConfigPathPart(priv);
}

tempo_config::ConfigPath::ConfigPath()
{
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
tempo_config::ConfigPath::isValid() const
{
    return m_priv != nullptr;
}

tempo_config::ConfigPath
tempo_config::ConfigPath::traverse(const ConfigPathPart &part) const
{
    if (m_priv == nullptr)
        return {};
    auto priv = std::make_shared<Priv>();
    priv->parts.insert(priv->parts.cbegin(), m_priv->parts.cbegin(), m_priv->parts.cend());
    priv->parts.push_back(part);
    return ConfigPath(priv);
}

tempo_config::ConfigPath
tempo_config::ConfigPath::root()
{
    auto priv = std::make_shared<Priv>();
    priv->parts.push_back(ConfigPathPart::root());
    return ConfigPath(priv);
}