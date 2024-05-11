
#include <absl/strings/str_cat.h>

#include <tempo_config/config_types.h>
#include <tempo_utils/log_message.h>

tempo_config::ConfigSource::ConfigSource()
    : m_type(ConfigSourceType::Invalid)
{
}

tempo_config::ConfigSource::ConfigSource(ConfigSourceType type, std::string_view source)
    : m_type(type),
      m_source(source)
{
}

tempo_config::ConfigSource::ConfigSource(const ConfigSource &other)
    : m_type(other.m_type),
      m_source(other.m_source)
{
}

bool
tempo_config::ConfigSource::isValid() const
{
    return m_type != ConfigSourceType::Invalid;
}

tempo_config::ConfigSourceType
tempo_config::ConfigSource::getType() const
{
    return m_type;
}

std::string
tempo_config::ConfigSource::getSource() const
{
    return m_source;
}

std::string_view
tempo_config::ConfigSource::sourceView() const
{
    return m_source;
}

tempo_config::ConfigLocation::ConfigLocation()
    : m_lineNumber(0),
      m_columnNumber(0),
      m_textOffset(0),
      m_textSpan(0)
{
}

tempo_config::ConfigLocation::ConfigLocation(
    std::shared_ptr<ConfigSource> source,
    tu_uint32 lineNumber,
    tu_uint32 columnNumber,
    tu_uint32 textOffset,
    tu_uint32 textSpan)
    : m_source(source),
      m_lineNumber(lineNumber),
      m_columnNumber(columnNumber),
      m_textOffset(textOffset),
      m_textSpan(textSpan)
{
    TU_ASSERT (m_source != nullptr);
}

tempo_config::ConfigLocation::ConfigLocation(const ConfigLocation &other)
    : m_source(other.m_source),
      m_lineNumber(other.m_lineNumber),
      m_columnNumber(other.m_columnNumber),
      m_textOffset(other.m_textOffset),
      m_textSpan(other.m_textSpan)
{
}

std::shared_ptr<tempo_config::ConfigSource>
tempo_config::ConfigLocation::getSource() const
{
    return m_source;
}

tu_uint32
tempo_config::ConfigLocation::getLineNumber() const
{
    return m_lineNumber;
}

tu_uint32
tempo_config::ConfigLocation::getColumnNumber() const
{
    return m_columnNumber;
}

tu_uint32
tempo_config::ConfigLocation::getTextOffset() const
{
    return m_textOffset;
}

tu_uint32
tempo_config::ConfigLocation::getTextSpan() const
{
    return m_textSpan;
}

std::string
tempo_config::ConfigLocation::toString() const
{
    if (m_source == nullptr)
        return "???";
    return absl::StrCat(m_source->sourceView(), ":", m_lineNumber, ":", m_columnNumber);
}

const char *
tempo_config::config_node_type_to_string(ConfigNodeType type)
{
    switch (type) {
        case ConfigNodeType::kNil:
            return "Nil";
        case ConfigNodeType::kValue:
            return "Value";
        case ConfigNodeType::kSeq:
            return "Seq";
        case ConfigNodeType::kMap:
            return "Map";
        default:
            return "???";
    }
}

tempo_config::ConfigNode::ConfigNode()
    : m_priv(std::shared_ptr<Priv>(
        new Priv{ConfigNodeType::kNil, {}}))
{
}

tempo_config::ConfigNode::ConfigNode(std::shared_ptr<Priv> priv)
    : m_priv(priv)
{
    TU_ASSERT (m_priv != nullptr);
}

const tempo_config::ConfigNode::Priv *
tempo_config::ConfigNode::getPriv() const
{
    return m_priv.get();
}

tempo_config::ConfigNode::ConfigNode(const ConfigNode &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_config::ConfigNode::isNil() const
{
    return m_priv->type == ConfigNodeType::kNil;
}

tempo_config::ConfigNodeType
tempo_config::ConfigNode::getNodeType() const
{
    return m_priv->type;
}

tempo_config::ConfigLocation
tempo_config::ConfigNode::getNodeLocation() const
{
    return m_priv->location;
}

bool
tempo_config::ConfigNode::operator==(const tempo_config::ConfigNode &other) const
{
    auto type = getNodeType();
    if (type != other.getNodeType())
        return false;
    switch (type) {
        case ConfigNodeType::kNil:
            return true;
        case ConfigNodeType::kValue:
            return toValue() == other.toValue();
        case ConfigNodeType::kSeq:
            return toSeq() == other.toSeq();
        case ConfigNodeType::kMap:
            return toMap() == other.toMap();
        default:
            return false;
    }
}

bool
tempo_config::ConfigNode::operator!=(const tempo_config::ConfigNode &other) const
{
    return !(*this == other);
}

tempo_config::ConfigValue
tempo_config::ConfigNode::toValue() const
{
    if (getNodeType() == ConfigNodeType::kValue)
        return *((ConfigValue *) this);
    return ConfigValue();
}

tempo_config::ConfigSeq
tempo_config::ConfigNode::toSeq() const
{
    if (getNodeType() == ConfigNodeType::kSeq)
        return *((ConfigSeq *) this);
    return ConfigSeq();
}

tempo_config::ConfigMap
tempo_config::ConfigNode::toMap() const
{
    if (getNodeType() == ConfigNodeType::kMap)
        return *((ConfigMap *) this);
    return ConfigMap();
}

tempo_config::ConfigNil::ConfigNil()
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new Priv{ConfigNodeType::kNil, {}}))
{
}

tempo_config::ConfigNil::ConfigNil(const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new Priv{ConfigNodeType::kNil, location}))
{
}

bool
tempo_config::ConfigNil::operator==(const tempo_config::ConfigNil &other) const
{
    return getPriv()->type == ConfigNodeType::kNil
        && other.getPriv()->type == ConfigNodeType::kNil;
}

bool
tempo_config::ConfigNil::operator!=(const tempo_config::ConfigNil &other) const
{
    return !(*this == other);
}

tempo_config::ConfigValue::ConfigValue()
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new ValuePriv{
            {ConfigNodeType::kValue, {}},
            std::make_shared<const std::string>()}))
{
}

tempo_config::ConfigValue::ConfigValue(const char *value, const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new ValuePriv{
            {ConfigNodeType::kValue, location},
            std::make_shared<const std::string>(value)}))
{
}

tempo_config::ConfigValue::ConfigValue(std::string_view value, const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new ValuePriv{
            {ConfigNodeType::kValue, location},
            std::make_shared<const std::string>(value)}))
{
}

tempo_config::ConfigValue::ConfigValue(std::string &&value, const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new ValuePriv{
            {ConfigNodeType::kValue, location},
            std::make_shared<const std::string>(std::move(value))}))
{
}

tempo_config::ConfigValue::ConfigValue(ConfigValueType value, const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new ValuePriv{
            {ConfigNodeType::kValue, location},
            value? value : std::make_shared<const std::string>()}))
{
}

std::string
tempo_config::ConfigValue::getValue() const
{
    auto *priv = (ValuePriv *) getPriv();
    return *priv->value;
}

bool
tempo_config::ConfigValue::operator==(const tempo_config::ConfigValue &other) const
{
    return getValue() == other.getValue();
}

bool
tempo_config::ConfigValue::operator!=(const tempo_config::ConfigValue &other) const
{
    return !(*this == other);
}

tempo_config::ConfigSeq::ConfigSeq()
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new SeqPriv{
            {ConfigNodeType::kSeq, {}},
            std::make_shared<const std::vector<ConfigNode>>()}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    const std::vector<ConfigNode> &seq,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new SeqPriv{
            {ConfigNodeType::kSeq, location},
            std::make_shared<const std::vector<ConfigNode>>(seq.cbegin(), seq.cend())}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    std::vector<ConfigNode> &&seq,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
    new SeqPriv{
        {ConfigNodeType::kSeq, location},
        std::make_shared<const std::vector<ConfigNode>>(std::move(seq))}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    std::initializer_list<ConfigNode> elements,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new SeqPriv{
            {ConfigNodeType::kSeq, location},
            std::make_shared<const std::vector<ConfigNode>>(elements)}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    ConfigSeqType seq,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new SeqPriv{
            {ConfigNodeType::kSeq, location},
            seq? seq : std::make_shared<const std::vector<ConfigNode>>()}))
{
}

bool
tempo_config::ConfigSeq::seqContains(int index) const
{
    auto *priv = (SeqPriv *) getPriv();
    return (0 <= index && index < static_cast<int>(priv->seq->size()));
}

bool
tempo_config::ConfigSeq::seqGet(int index, ConfigNode &node) const
{
    auto *priv = (SeqPriv *) getPriv();
    if (0 <= index && index < static_cast<int>(priv->seq->size())) {
        node = priv->seq->at(index);
        return true;
    }
    return false;
}

tempo_config::ConfigNode
tempo_config::ConfigSeq::seqAt(int index) const
{
    ConfigNode node;
    if (seqGet(index, node))
        return node;
    return {};
}

int
tempo_config::ConfigSeq::seqSize() const
{
    auto *priv = (SeqPriv *) getPriv();
    return priv->seq->size();
}

std::vector<tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigSeq::seqBegin() const
{
    auto *priv = (SeqPriv *) getPriv();
    return priv->seq->cbegin();
}

std::vector<tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigSeq::seqEnd() const
{
    auto *priv = (SeqPriv *) getPriv();
    return priv->seq->cend();
}

bool
tempo_config::ConfigSeq::operator==(const tempo_config::ConfigSeq &other) const
{
    return std::equal(seqBegin(), seqEnd(), other.seqBegin(), other.seqEnd());
}

bool
tempo_config::ConfigSeq::operator!=(const tempo_config::ConfigSeq &other) const
{
    return !(*this == other);
}

tempo_config::ConfigSeq
tempo_config::ConfigSeq::append(const ConfigSeq &dst, const ConfigNode &src)
{
    auto seq = std::make_shared<std::vector<ConfigNode>>(dst.seqBegin(), dst.seqEnd());
    seq->push_back(src);
    return ConfigSeq(seq);
}

tempo_config::ConfigSeq
tempo_config::ConfigSeq::extend(const ConfigSeq &dst, const ConfigSeq &src)
{
    auto seq = std::make_shared<std::vector<ConfigNode>>(dst.seqBegin(), dst.seqEnd());
    seq->insert(seq->cend(), src.seqBegin(), src.seqEnd());
    return ConfigSeq(seq);
}

tempo_config::ConfigMap::ConfigMap()
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new MapPriv{
            {ConfigNodeType::kMap, {}},
            std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>()}))
{
}

tempo_config::ConfigMap::ConfigMap(
    const absl::flat_hash_map<std::string,ConfigNode> &map,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new MapPriv{
            {ConfigNodeType::kMap, location},
            std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>(map.cbegin(), map.cend())}))
{
}

tempo_config::ConfigMap::ConfigMap(
    absl::flat_hash_map<std::string,ConfigNode> &&map,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new MapPriv{
            {ConfigNodeType::kMap, location},
            std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>(std::move(map))}))
{
}

tempo_config::ConfigMap::ConfigMap(
    std::initializer_list<std::pair<std::string,ConfigNode>> entries,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new MapPriv{
            {ConfigNodeType::kMap, location},
            std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>(entries)}))
{
}

tempo_config::ConfigMap::ConfigMap(
    ConfigMapType map,
    const ConfigLocation &location)
    : ConfigNode(std::shared_ptr<ConfigNode::Priv>(
        new MapPriv{
            {ConfigNodeType::kMap, location},
            map? map : std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>()}))
{
}

bool
tempo_config::ConfigMap::mapContains(std::string_view key) const
{
    auto *priv = (MapPriv *) getPriv();
    return priv->map->contains(key);
}

bool
tempo_config::ConfigMap::mapGet(std::string_view key, ConfigNode &node) const
{
    auto *priv = (MapPriv *) getPriv();
    if (priv->map->contains(key)) {
        node = priv->map->at(key);
        return true;
    }
    return false;
}

tempo_config::ConfigNode
tempo_config::ConfigMap::mapAt(std::string_view key) const
{
    ConfigNode node;
    if (mapGet(key, node))
        return node;
    return {};
}

int
tempo_config::ConfigMap::mapSize() const
{
    auto *priv = (MapPriv *) getPriv();
    return priv->map->size();
}

absl::flat_hash_map<std::string,tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigMap::mapBegin() const
{
    auto *priv = (MapPriv *) getPriv();
    return priv->map->cbegin();
}

absl::flat_hash_map<std::string,tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigMap::mapEnd() const
{
    auto *priv = (MapPriv *) getPriv();
    return priv->map->cend();
}

bool
tempo_config::ConfigMap::operator==(const tempo_config::ConfigMap &other) const
{
    for (auto iterator = mapBegin(); iterator != mapEnd(); iterator++) {
        tempo_config::ConfigNode value;
        if (!other.mapGet(iterator->first, value))
            return false;
        if (value != iterator->second)
            return false;
    }
    return true;
}

bool
tempo_config::ConfigMap::operator!=(const tempo_config::ConfigMap &other) const
{
    return !(*this == other);
}

/**
 * Constructs a new ConfigMap containing all entries from dst and any entries in src which are not present
 * in src (i.e. no entries from dst are overwritten).
 *
 * @param dst The destination ConfigMap.
 * @param src The source ConfigMap.
 * @return The new ConfigMap.
 */
tempo_config::ConfigMap
tempo_config::ConfigMap::extend(const ConfigMap &dst, const ConfigMap &src)
{
    auto map = std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>(dst.mapBegin(), dst.mapEnd());
    for (auto iterator = src.mapBegin(); iterator != src.mapEnd(); iterator++) {
        if (!map->contains(iterator->first)) {
            map->insert_or_assign(iterator->first, iterator->second);
        }
    }
    return ConfigMap(map);
}

/**
 * Constructs a new ConfigMap containing all entries from src and any entries in dst which are not present
 * in src (i.e. any key in dst which is present in src will be overwritten).
 *
 * @param dst The destination ConfigMap.
 * @param src The source ConfigMap.
 * @return The new ConfigMap.
 */
tempo_config::ConfigMap
tempo_config::ConfigMap::update(const ConfigMap &dst, const ConfigMap &src)
{
    auto map = std::make_shared<absl::flat_hash_map<std::string,ConfigNode>>(dst.mapBegin(), dst.mapEnd());
    map->insert(src.mapBegin(), src.mapEnd());
    return ConfigMap(map);
}

static std::string
node_to_json(const tempo_config::ConfigNode &node)
{
    switch (node.getNodeType()) {

        case tempo_config::ConfigNodeType::kNil:
            return "null";

        case tempo_config::ConfigNodeType::kValue:
            return absl::StrCat("\"", node.toValue().getValue(), "\"");

        case tempo_config::ConfigNodeType::kSeq: {
            auto seq = node.toSeq();
            std::string s;
            s.push_back('[');
            auto iterator = seq.seqBegin();
            if (iterator != seq.seqEnd()) {
                s.append(iterator->toString());
                for (++iterator; iterator != seq.seqEnd(); iterator++) {
                    s.push_back(',');
                    s.append(iterator->toString());
                }
            }
            s.push_back(']');
            return s;
        }

        case tempo_config::ConfigNodeType::kMap: {
            auto map = node.toMap();
            std::string s;
            s.push_back('{');
            auto iterator = map.mapBegin();
            if (iterator != map.mapEnd()) {
                s.push_back('"');
                s.append(iterator->first);
                s.push_back('"');
                s.push_back(':');
                s.append(iterator->second.toString());
                for (++iterator; iterator != map.mapEnd(); iterator++) {
                    s.push_back(',');
                    s.push_back('"');
                    s.append(iterator->first);
                    s.push_back('"');
                    s.push_back(':');
                    s.append(iterator->second.toString());
                }
            }
            s.push_back('}');
            return s;
        }

        default:
            return {};
    }
}

/**
 * Returns a JSON-serialized string containing the contents of the ConfigNode.
 *
 * @return The JSON-serialized string.
 */
std::string
tempo_config::ConfigNode::toString() const
{
    return node_to_json(*this);
}

tempo_config::ConfigFile::ConfigFile()
{
}

tempo_config::ConfigFile::ConfigFile(const std::filesystem::path &path, const ConfigNode &root)
    : m_path(path),
      m_root(root)
{
}

bool
tempo_config::ConfigFile::isValid() const
{
    return !m_path.empty();
}

std::filesystem::path
tempo_config::ConfigFile::getPath() const
{
    return m_path;
}

tempo_config::ConfigNode
tempo_config::ConfigFile::getRoot() const
{
    return m_root;
}
