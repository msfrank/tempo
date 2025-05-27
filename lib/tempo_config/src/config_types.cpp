
#include <absl/strings/str_cat.h>

#include <tempo_config/config_types.h>
#include <tempo_utils/log_message.h>

#include "../../tempo_schema/include/tempo_schema/schema_resource.h"

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
    : m_data(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kNil,
        ConfigLocation{},
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigNode::ConfigNode(std::shared_ptr<ConfigNodeData> data)
    : m_data(std::move(data))
{
    TU_ASSERT (m_data != nullptr);
}

tempo_config::ConfigNode::ConfigNode(const ConfigNode &other)
    : m_data(other.m_data)
{
}

tempo_config::ConfigNode::ConfigNode(ConfigNode &&other) noexcept
{
    m_data.swap(other.m_data);
}

tempo_config::ConfigNode&
tempo_config::ConfigNode::operator=(const ConfigNode &other)
{
    if (this != &other) {
        m_data = other.m_data;
    }
    return *this;
}

tempo_config::ConfigNode&
tempo_config::ConfigNode::operator=(ConfigNode &&other) noexcept
{
    if (this != &other) {
        m_data.swap(other.m_data);
    }
    return *this;
}

bool
tempo_config::ConfigNode::isNil() const
{
    return m_data->type == ConfigNodeType::kNil;
}

tempo_config::ConfigNodeType
tempo_config::ConfigNode::getNodeType() const
{
    return m_data->type;
}

tempo_config::ConfigLocation
tempo_config::ConfigNode::getNodeLocation() const
{
    return m_data->location;
}
//
// template<typename IterType>
// inline int cmp(IterType it1, IterType it2, IterType end1, IterType end2)
// {
//
// }

typedef std::vector<tempo_config::ConfigNode> ConfigNodeVector;
typedef absl::btree_map<std::string,tempo_config::ConfigNode> ConfigNodeBtree;

int compare_seq(const ConfigNodeVector &seq1, const ConfigNodeVector &seq2)
{
    if (seq1.empty() && seq2.empty())
        return 0;
    auto it1 = seq1.cbegin();
    auto it2 = seq2.cbegin();
    auto end1 = seq1.cend();
    auto end2 = seq2.cend();
    while (it1 != end1 && it2 != end2) {
        auto cmp = it1->compare(*it2);
        if (cmp != 0)
            return cmp;
        ++it1, ++it2;
    }
    // at this point either one iterator or both iterators are exhausted
    if (it1 == end1) {
        // if it1 and it2 are exhausted return 0 (equal), otherwise return -1 (it1 is less than it2)
        return it2 == end2? 0 : -1;
    }
    // it1 is not exhausted, so it1 must be greater than it2
    return 1;
}

int compare_map(const ConfigNodeBtree &map1, const ConfigNodeBtree &map2)
{
    if (map1.empty() && map2.empty())
        return 0;
    auto it1 = map1.cbegin();
    auto it2 = map2.cbegin();
    auto end1 = map1.cend();
    auto end2 = map2.cend();
    while (it1 != end1 && it2 != end2) {
        const auto &key1 = it1->first;
        const auto &key2 = it2->first;
        auto keycmp = key1.compare(key2);
        if (keycmp != 0)
            return keycmp;
        const auto &val1 = it1->second;
        const auto &val2 = it2->second;
        auto valcmp = val1.compare(val2);
        if (valcmp != 0)
            return valcmp;
        ++it1, ++it2;
    }
    // at this point either one iterator or both iterators are exhausted
    if (it1 == end1) {
        // if it1 and it2 are exhausted return 0 (equal), otherwise return -1 (it1 is less than it2)
        return it2 == end2? 0 : -1;
    }
    // it1 is not exhausted, so it1 must be greater than it2
    return 1;
}

int
tempo_config::ConfigNode::compare(const ConfigNode &other) const
{
    auto ltype = getNodeType();
    auto rtype = other.getNodeType();

    if (ltype != rtype)
        return ltype < rtype? -1 : 1;

    switch (ltype) {
        case ConfigNodeType::kNil:
            return 0;
        case ConfigNodeType::kValue:
            return m_data->value.compare(other.m_data->value);
        case ConfigNodeType::kSeq:
            return compare_seq(m_data->seq, other.m_data->seq);
        case ConfigNodeType::kMap:
            return compare_map(m_data->map, other.m_data->map);
        default:
            TU_UNREACHABLE();
    }
}

void
tempo_config::ConfigNode::hash(absl::HashState state) const
{
    switch (m_data->type) {
        case ConfigNodeType::kNil:
            break;;
        case ConfigNodeType::kValue: {
            state = absl::HashState::combine(std::move(state), m_data->value);
            break;;
        }
        case ConfigNodeType::kSeq: {
            state = absl::HashState::combine(std::move(state), m_data->seq);
            break;
        }
        case ConfigNodeType::kMap: {
            state = absl::HashState::combine(std::move(state), m_data->map);
            break;
        }
    }
    absl::HashState::combine(std::move(state), m_data->type);
}

bool
tempo_config::ConfigNode::operator==(const ConfigNode &other) const
{
    return compare(other) == 0;
}

bool
tempo_config::ConfigNode::operator!=(const ConfigNode &other) const
{
    return compare(other) != 0;
}

bool
tempo_config::ConfigNode::operator<(const ConfigNode &other) const
{
    return compare(other) < 0;
}

tempo_config::ConfigValue
tempo_config::ConfigNode::toValue() const
{
    if (getNodeType() == ConfigNodeType::kValue)
        return ConfigValue(m_data);
    return {};
}

tempo_config::ConfigSeq
tempo_config::ConfigNode::toSeq() const
{
    if (getNodeType() == ConfigNodeType::kSeq)
        return ConfigSeq(m_data);
    return {};
}

tempo_config::ConfigMap
tempo_config::ConfigNode::toMap() const
{
    if (getNodeType() == ConfigNodeType::kMap)
        return ConfigMap(m_data);
    return {};
}

tempo_config::ConfigNode
tempo_config::ConfigNode::toNode() const
{
    return *this;
}

tempo_config::ConfigNil::ConfigNil()
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kNil,
        ConfigLocation{},
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigNil::ConfigNil(const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kNil,
        location,
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigNil::ConfigNil(std::shared_ptr<ConfigNodeData> data)
    : ConfigNode(std::move(data))
{
}

bool
tempo_config::ConfigNil::operator==(const ConfigNil &other) const
{
    return compare(other) == 0;
}

bool
tempo_config::ConfigNil::operator!=(const tempo_config::ConfigNil &other) const
{
    return compare(other) != 0;
}

bool
tempo_config::ConfigNil::operator<(const tempo_config::ConfigNil &other) const
{
    return compare(other) < 0;
}

tempo_config::ConfigValue::ConfigValue()
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kValue,
        ConfigLocation{},
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigValue::ConfigValue(const char *value, const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kValue,
        location,
        std::string(value),
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigValue::ConfigValue(std::string_view value, const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kValue,
        location,
        std::string(value),
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigValue::ConfigValue(std::string &&value, const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kValue,
        location,
        std::move(value),
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigValue::ConfigValue(std::shared_ptr<ConfigNodeData> data)
    : ConfigNode(std::move(data))
{
}

std::string
tempo_config::ConfigValue::getValue() const
{
    return m_data->value;
}

bool
tempo_config::ConfigValue::operator==(const ConfigValue &other) const
{
    return compare(other) == 0;
}

bool
tempo_config::ConfigValue::operator!=(const ConfigValue &other) const
{
    return compare(other) != 0;
}

bool
tempo_config::ConfigValue::operator<(const ConfigValue &other) const
{
    return compare(other) < 0;
}

tempo_config::ConfigSeq::ConfigSeq()
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kSeq,
        ConfigLocation{},
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    const std::vector<ConfigNode> &seq,
    const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kSeq,
        location,
        std::string{},
        std::vector(seq.cbegin(), seq.cend()),
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    std::vector<ConfigNode> &&seq,
    const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kSeq,
        location,
        std::string{},
        std::vector(std::move(seq)),
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigSeq::ConfigSeq(
    std::initializer_list<ConfigNode> elements,
    const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kSeq,
        location,
        std::string{},
        std::vector(elements),
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigSeq::ConfigSeq(std::shared_ptr<ConfigNodeData> data)
    : ConfigNode(std::move(data))
{
}

bool
tempo_config::ConfigSeq::seqContains(int index) const
{
    return 0 <= index && index < static_cast<int>(m_data->seq.size());
}

bool
tempo_config::ConfigSeq::seqGet(int index, ConfigNode &node) const
{
    if (0 <= index && index < static_cast<int>(m_data->seq.size())) {
        node = m_data->seq.at(index);
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
    return m_data->seq.size();
}

std::vector<tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigSeq::seqBegin() const
{
    return m_data->seq.cbegin();
}

std::vector<tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigSeq::seqEnd() const
{
    return m_data->seq.cend();
}

bool
tempo_config::ConfigSeq::operator==(const ConfigSeq &other) const
{
    return compare(other) == 0;
}

bool
tempo_config::ConfigSeq::operator!=(const ConfigSeq &other) const
{
    return compare(other) != 0;
}

bool
tempo_config::ConfigSeq::operator<(const ConfigSeq &other) const
{
    return compare(other) < 0;
}

tempo_config::ConfigSeq
tempo_config::ConfigSeq::append(const ConfigSeq &dst, const ConfigNode &src)
{
    std::vector seq(dst.seqBegin(), dst.seqEnd());
    seq.push_back(src);
    return ConfigSeq(std::move(seq));
}

tempo_config::ConfigSeq
tempo_config::ConfigSeq::extend(const ConfigSeq &dst, const ConfigSeq &src)
{
    std::vector seq(dst.seqBegin(), dst.seqEnd());
    seq.insert(seq.cend(), src.seqBegin(), src.seqEnd());
    return ConfigSeq(std::move(seq));
}

tempo_config::ConfigMap::ConfigMap()
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kMap,
        ConfigLocation{},
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>{}))
{
}

tempo_config::ConfigMap::ConfigMap(
    const absl::btree_map<std::string,ConfigNode> &map,
    const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kMap,
        location,
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>(map.cbegin(), map.cend())))
{
}

tempo_config::ConfigMap::ConfigMap(
    absl::btree_map<std::string,ConfigNode> &&map,
    const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kMap,
        location,
        std::string{},
        std::vector<ConfigNode>{},
        std::move(map)))
{
}

tempo_config::ConfigMap::ConfigMap(
    const absl::flat_hash_map<std::string,ConfigNode> &map,
    const ConfigLocation &location)
    : ConfigNode(std::make_shared<ConfigNodeData>(
        ConfigNodeType::kMap,
        location,
        std::string{},
        std::vector<ConfigNode>{},
        absl::btree_map<std::string,ConfigNode>(map.cbegin(), map.cend())))
{
}

tempo_config::ConfigMap::ConfigMap(
    std::initializer_list<std::pair<std::string,ConfigNode>> entries,
    const ConfigLocation &location)
: ConfigNode(std::make_shared<ConfigNodeData>(
    ConfigNodeType::kMap,
    location,
    std::string{},
    std::vector<ConfigNode>{},
    absl::btree_map<std::string,ConfigNode>{entries}))
{
}

tempo_config::ConfigMap::ConfigMap(std::shared_ptr<ConfigNodeData> data)
    : ConfigNode(std::move(data))
{
}

bool
tempo_config::ConfigMap::mapContains(std::string_view key) const
{
    return m_data->map.contains(key);
}

bool
tempo_config::ConfigMap::mapGet(std::string_view key, ConfigNode &node) const
{
    auto entry = m_data->map.find(key);
    if (entry != m_data->map.cend()) {
        node = entry->second;
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
    return m_data->map.size();
}

absl::btree_map<std::string,tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigMap::mapBegin() const
{
    return m_data->map.cbegin();
}

absl::btree_map<std::string,tempo_config::ConfigNode>::const_iterator
tempo_config::ConfigMap::mapEnd() const
{
    return m_data->map.cend();
}

bool
tempo_config::ConfigMap::operator==(const ConfigMap &other) const
{
    return compare(other) == 0;
}

bool
tempo_config::ConfigMap::operator!=(const ConfigMap &other) const
{
    return compare(other) != 0;
}

bool
tempo_config::ConfigMap::operator<(const ConfigMap &other) const
{
    return compare(other) < 0;
}

// void
// tempo_config::ConfigMap::hash(absl::HashState state) const
// {
//     auto *priv = (MapPriv *) getPriv();
//     state = absl::HashState::combine(std::move(state), priv->type);
//     std::vector<std::pair<std::string, ConfigNode>> entries(priv->map->begin(), priv->map->end());
//     std::sort(entries.begin(), entries.end(), [](auto &a, auto &b) -> bool {
//         return a.first < b.first;
//     });
//     absl::HashState::combine(std::move(state), entries);
// }

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
    absl::btree_map<std::string,ConfigNode> map(dst.mapBegin(), dst.mapEnd());
    for (auto iterator = src.mapBegin(); iterator != src.mapEnd(); iterator++) {
        if (!map.contains(iterator->first)) {
            map.insert_or_assign(iterator->first, iterator->second);
        }
    }
    return ConfigMap(std::move(map));
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
    absl::btree_map<std::string,ConfigNode> map(dst.mapBegin(), dst.mapEnd());
    map.insert(src.mapBegin(), src.mapEnd());
    return ConfigMap(std::move(map));
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
