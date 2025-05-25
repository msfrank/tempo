
#include "tempo_config/config_builder.h"

#include <tempo_config/config_builder.h>

tempo_config::ConfigSeqBuilder::ConfigSeqBuilder()
    : m_priv(std::make_shared<Priv>())
{
}

tempo_config::ConfigSeqBuilder
tempo_config::ConfigSeqBuilder::append(const ConfigNil &nil)
{
    m_priv->seq.push_back(nil.toNode());
    return *this;
}

tempo_config::ConfigSeqBuilder
tempo_config::ConfigSeqBuilder::append(const ConfigValue &value)
{
    m_priv->seq.push_back(value.toNode());
    return *this;
}

tempo_config::ConfigSeqBuilder
tempo_config::ConfigSeqBuilder::append(const ConfigSeq &seq)
{
    m_priv->seq.push_back(seq.toNode());
    return *this;
}

tempo_config::ConfigSeqBuilder
tempo_config::ConfigSeqBuilder::append(const ConfigMap &map)
{
    m_priv->seq.push_back(map.toNode());
    return *this;
}

tempo_config::ConfigSeqBuilder
tempo_config::ConfigSeqBuilder::append(const ConfigNode &node)
{
    m_priv->seq.push_back(node);
    return *this;
}

tempo_config::ConfigNode
tempo_config::ConfigSeqBuilder::buildNode()
{
    ConfigSeq seq(m_priv->seq);
    return seq.toNode();
}

tempo_config::ConfigSeq
tempo_config::ConfigSeqBuilder::buildSeq()
{
    return ConfigSeq(m_priv->seq);
}

tempo_config::ConfigMapBuilder::ConfigMapBuilder()
    : m_priv(std::make_shared<Priv>())
{
}

tempo_config::ConfigMapBuilder
tempo_config::ConfigMapBuilder::put(std::string_view name, const ConfigNil &nil)
{
    m_priv->map[name] = nil.toNode();
    return *this;
}

tempo_config::ConfigMapBuilder
tempo_config::ConfigMapBuilder::put(std::string_view name, const ConfigValue &value)
{
    m_priv->map[name] = value.toNode();
    return *this;
}

tempo_config::ConfigMapBuilder
tempo_config::ConfigMapBuilder::put(std::string_view name, const ConfigSeq &seq)
{
    m_priv->map[name] = seq.toNode();
    return *this;
}

tempo_config::ConfigMapBuilder
tempo_config::ConfigMapBuilder::put(std::string_view name, const ConfigMap &map)
{
    m_priv->map[name] = map.toNode();
    return *this;
}

tempo_config::ConfigMapBuilder
tempo_config::ConfigMapBuilder::put(std::string_view name, const ConfigNode &node)
{
    m_priv->map[name] = node;
    return *this;
}

tempo_config::ConfigNode
tempo_config::ConfigMapBuilder::buildNode()
{
    ConfigMap map(m_priv->map);
    return map.toNode();
}

tempo_config::ConfigMap
tempo_config::ConfigMapBuilder::buildMap()
{
    return ConfigMap(m_priv->map);
}

tempo_config::ConfigNode
tempo_config::nilNode()
{
    ConfigNil node;
    return static_cast<ConfigNode>(node);
}

tempo_config::ConfigNode
tempo_config::valueNode(std::string_view value)
{
    ConfigValue node(value);
    return static_cast<ConfigNode>(node);
}

tempo_config::ConfigSeqBuilder
tempo_config::startSeq()
{
    return ConfigSeqBuilder();
}

tempo_config::ConfigMapBuilder
tempo_config::startMap()
{
    return ConfigMapBuilder();
}

