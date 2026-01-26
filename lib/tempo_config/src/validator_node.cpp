
#include <absl/strings/str_join.h>
#include <re2/re2.h>

#include <tempo_config/config_result.h>
#include <tempo_config/validator_node.h>
#include <tempo_utils/logging.h>

tempo_config::ValidatorNodeType
tempo_config::ValidateAny::getType() const
{
    return ValidatorNodeType::Any;
}

tempo_utils::Status
tempo_config::ValidateAny::validate(const ConfigNode &node)
{
    return {};
}

tempo_config::ValidatorNodeType
tempo_config::ValidateNone::getType() const
{
    return ValidatorNodeType::None;
}

tempo_utils::Status
tempo_config::ValidateNone::validate(const ConfigNode &node)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "expected none, encountered {}", node.toString());
}

// tempo_config::ValidatorComparisonType
// tempo_config::ValidateAny::compare(std::shared_ptr<ValidatorNode> other) const
// {
//     if (!other || other->getType() == ValidatorNodeType::SCHEMA_EMPTY)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//     if (other->getType() == ValidatorNodeType::SCHEMA_ANY)
//         return ValidatorComparisonType::COMPARE_EQUAL;
//     return ValidatorComparisonType::COMPARE_SUBSET;
// }

// ValidatorConst::ValidatorConst() : ValidatorNode()
// {
// }
//
// ValidatorNodeType
// ValidatorConst::getType() const
// {
//     return ValidatorNodeType::SCHEMA_CONST;
// }
//
// ValidatorValueType
// atom_type_to_value_type(AtomType atomType)
// {
//     switch (atomType) {
//         case AtomType::NULL_TYPE:
//             return ValidatorValueType::VALUE_NULL;
//         case AtomType::BOOLEAN_TYPE:
//             return ValidatorValueType::VALUE_BOOL;
//         case AtomType::INTEGER_TYPE:
//             return ValidatorValueType::VALUE_INTEGER;
//         case AtomType::FLOAT_TYPE:
//             return ValidatorValueType::VALUE_FLOAT;
//         case AtomType::STRING_TYPE:
//             return ValidatorValueType::VALUE_STRING;
//         case AtomType::SEQ_TYPE:
//             return ValidatorValueType::VALUE_SEQ;
//         case AtomType::MAP_TYPE:
//             return ValidatorValueType::VALUE_MAP;
//         default:
//             break;
//     }
//     return ValidatorValueType::VALUE_EMPTY;
// }
//
// ValidatorValueType
// ValidatorConst::getValueType() const
// {
//     return atom_type_to_value_type(m_const.getType());
// }
//
// Atom
// ValidatorConst::getConst() const
// {
//     return m_const;
// }
//
// void
// ValidatorConst::setConst(const Atom &const_)
// {
//     m_const = const_;
// }
//
// SideEffect
// ValidatorConst::validate(const Atom &atom)
// {
//     if (atom == m_const)
//         return SideEffect::success();
//     return SideEffect::failure(RuntimeError(QString("%1 is not const %2").arg(atom.toString()).arg(m_const.toString())));
// }
//
// ValidatorComparisonType
// ValidatorConst::compare(ValidatorNode *other) const
// {
//     if (!other)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//     if (other->getType() == ValidatorNodeType::SCHEMA_CONST && m_const == static_cast<ValidatorConst *>(other)->getConst())
//         return ValidatorComparisonType::COMPARE_EQUAL;
//     return ValidatorComparisonType::COMPARE_DISJOINT;
// }
//
// uint
// ValidatorConst::hash() const
// {
//     return qHash(m_const);
// }
//
// ValidatorNode *
// ValidatorConst::clone() const
// {
//     auto const_ = new ValidatorConst();
//     const_->m_const = m_const;
//     return const_;
// }
//
// ValidatorEnum::ValidatorEnum() : ValidatorNode()
// {
// }
//
// ValidatorNodeType
// ValidatorEnum::getType() const
// {
//     return ValidatorNodeType::SCHEMA_ENUM;
// }
//
// ValidatorValueType
// ValidatorEnum::getValueType() const
// {
//     if (m_enum.isEmpty())
//         return ValidatorValueType::VALUE_EMPTY;
//     //auto enum_ = m_enum.toList();
//     auto enum_ = QList<Atom>(m_enum.cbegin(), m_enum.cend());
//     ValidatorValueType valueType = atom_type_to_value_type(enum_.first().getType());
//     if (m_enum.size() == 1)
//         return valueType;
//     for (int i = 1; i < m_enum.size(); i++) {
//         ValidatorValueType otherType = atom_type_to_value_type(enum_[i].getType());
//         if (otherType != valueType)
//             return ValidatorValueType::VALUE_ANY;
//     }
//     return valueType;
// }
//
// QSet<Atom>
// ValidatorEnum::getEnum() const
// {
//     return m_enum;
// }
//
// void
// ValidatorEnum::setEnum(const QSet<Atom> &enum_)
// {
//     m_enum = enum_;
// }
//
// SideEffect
// ValidatorEnum::validate(const Atom &atom)
// {
//     if (m_enum.contains(atom))
//         return SideEffect::success();
//     QStringList sl;
//     for (auto iterator = m_enum.cbegin(); iterator != m_enum.cend(); iterator++) {
//         sl.append(iterator->toString());
//     }
//     return SideEffect::failure(RuntimeError(QString("%1 not in enum [%2]").arg(atom.toString()).arg(sl.join(','))));
// }
//
// ValidatorComparisonType
// ValidatorEnum::compare(ValidatorNode *other) const
// {
//     if (!other)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//     if (other->getType() == ValidatorNodeType::SCHEMA_CONST) {
//         auto c = static_cast<ValidatorConst *>(other)->getConst();
//         if (m_enum.contains(c))
//             return ValidatorComparisonType::COMPARE_SUBSET;
//     }
//     if (other->getType() == ValidatorNodeType::SCHEMA_ENUM) {
//         if (m_enum == static_cast<ValidatorEnum *>(other)->getEnum())
//             return ValidatorComparisonType::COMPARE_EQUAL;
//     }
//     return ValidatorComparisonType::COMPARE_DISJOINT;
// }
//
// uint
// ValidatorEnum::hash() const
// {
//     uint result = 0;
//     for (const auto &e : m_enum) {
//         result += qHash(e);
//     }
//     return result;
// }
//
// ValidatorNode *
// ValidatorEnum::clone() const
// {
//     auto enum_ = new ValidatorEnum();
//     enum_->m_enum = m_enum;
//     return enum_;
// }

tempo_config::ValidateInteger::ValidateInteger(
    tu_int64 minimum,
    tu_int64 maximum,
    tu_int64 multipleOf,
    bool minExclusive,
    bool maxExclusive)
    : m_minimum(minimum),
      m_maximum(maximum),
      m_multipleOf(multipleOf),
      m_minExclusive(minExclusive),
      m_maxExclusive(maxExclusive)
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateInteger::getType() const
{
    return ValidatorNodeType::Integer;
}

tu_int64
tempo_config::ValidateInteger::getMinimum() const
{
    return m_minimum;
}

tu_int64
tempo_config::ValidateInteger::getMaximum() const
{
    return m_maximum;
}

tu_int64
tempo_config::ValidateInteger::getMultipleOf() const
{
    return m_multipleOf;
}

bool
tempo_config::ValidateInteger::getMinExclusive() const
{
    return m_minExclusive;
}

bool
tempo_config::ValidateInteger::getMaxExclusive() const
{
    return m_maxExclusive;
}

// tempo_config::ValidatorComparisonType
// tempo_config::ValidateInteger::compare(ValidatorNode *other) const
// {
//     if (!other)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//
//     if (other->getType() == ValidatorNodeType::SCHEMA_INTEGER) {
//         auto *otherInteger = static_cast<ValidateInteger *>(other);
//
//         if (m_minimum == otherInteger->getMinimum() &&
//             m_minExclusive == otherInteger->getMinExclusive() &&
//             m_maximum == otherInteger->getMaximum() &&
//             m_maxExclusive == otherInteger->getMaxExclusive() &&
//             m_multipleOf == otherInteger->getMultipleOf())
//             return ValidatorComparisonType::COMPARE_EQUAL;
//
//         if ((m_minimum <= otherInteger->getMinimum()
//             || (m_minimum == otherInteger->getMinimum() && otherInteger->getMinExclusive()))
//             && (m_maximum >= otherInteger->getMaximum()
//             || (m_maximum == otherInteger->getMaximum() && otherInteger->getMaxExclusive()))
//             && m_multipleOf == otherInteger->getMultipleOf())
//             return ValidatorComparisonType::COMPARE_SUBSET;
//
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     QSet<Atom> values;
//     switch (other->getType()) {
//         case ValidatorNodeType::SCHEMA_CONST:
//             values = {static_cast<ValidatorConst *>(other)->getConst()};
//             break;
//         case ValidatorNodeType::SCHEMA_ENUM:
//             values = static_cast<ValidatorEnum *>(other)->getEnum();
//             break;
//         default:
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     for (const auto &value : values) {
//         if (!value.isInteger())
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//         auto i64 = value.getInteger();
//         if (i64 < m_minimum
//             || (m_minimum == i64 && m_minExclusive)
//             || i64 > m_maximum
//             || (m_maximum == i64 && m_maxExclusive)
//             || (i64 % m_multipleOf != 0))
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     return ValidatorComparisonType::COMPARE_SUBSET;
// }

tempo_utils::Status
tempo_config::ValidateInteger::validate(const ConfigNode &node)
{
    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a valid integer",node.toString());
    auto value = node.toValue();

    tu_int64 i64;
    if (!absl::SimpleAtoi(value.getValue(), &i64))
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a valid integer",node.toString());

    if (i64 < m_minimum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is less than {}", i64, m_minimum);
    if (m_minExclusive && i64 == m_minimum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is equal to {}", i64, m_minimum);
    if (i64 > m_maximum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is greater than {}", i64, m_maximum);
    if (m_maxExclusive && i64 == m_maximum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is equal to {}", i64, m_maximum);
    if (m_multipleOf != 1 && i64 % m_multipleOf != 0)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a multiple of {}", i64, m_multipleOf);
    return {};
}


tempo_config::ValidateFloat::ValidateFloat(
    double minimum,
    double maximum,
    bool minExclusive,
    bool maxExclusive)
    : m_minimum(minimum),
      m_maximum(maximum),
      m_minExclusive(minExclusive),
      m_maxExclusive(maxExclusive)
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateFloat::getType() const
{
    return ValidatorNodeType::Float;
}

double
tempo_config::ValidateFloat::getMinimum() const
{
    return m_minimum;
}

double
tempo_config::ValidateFloat::getMaximum() const
{
    return m_maximum;
}

bool
tempo_config::ValidateFloat::getMinExclusive() const
{
    return m_minExclusive;
}

bool
tempo_config::ValidateFloat::getMaxExclusive() const
{
    return m_maxExclusive;
}

tempo_utils::Status
tempo_config::ValidateFloat::validate(const ConfigNode &node)
{
    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a valid integer",node.toString());
    auto value = node.toValue();

    double dbl;
    if (!absl::SimpleAtod(value.getValue(), &dbl))
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a valid float",node.toString());

    if (dbl < m_minimum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is less than {}", dbl, m_minimum);
    if (m_minExclusive && dbl == m_minimum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is equal to {}", dbl, m_minimum);
    if (dbl > m_maximum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is greater than {}", dbl, m_maximum);
    if (m_maxExclusive && dbl == m_maximum)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is equal to {}", dbl, m_maximum);
    return {};
}

// ValidatorComparisonType
// ValidateFloat::compare(ValidatorNode *other) const
// {
//     if (!other)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//
//     if (other->getType() == ValidatorNodeType::SCHEMA_FLOAT) {
//         auto *otherFloat = static_cast<ValidateFloat *>(other);
//
//         if (m_minimum == otherFloat->getMinimum() &&
//             m_minExclusive == otherFloat->getMinExclusive() &&
//             m_maximum == otherFloat->getMaximum() &&
//             m_maxExclusive == otherFloat->getMaxExclusive())
//             return ValidatorComparisonType::COMPARE_EQUAL;
//
//         if ((m_minimum <= otherFloat->getMinimum()
//             || (m_minimum == otherFloat->getMinimum() && otherFloat->getMinExclusive()))
//             && (m_maximum >= otherFloat->getMaximum()
//             || (m_maximum == otherFloat->getMaximum() && otherFloat->getMaxExclusive())))
//             return ValidatorComparisonType::COMPARE_SUBSET;
//
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     QSet<Atom> values;
//     switch (other->getType()) {
//         case ValidatorNodeType::SCHEMA_CONST:
//             values = {static_cast<ValidatorConst *>(other)->getConst()};
//             break;
//         case ValidatorNodeType::SCHEMA_ENUM:
//             values = static_cast<ValidatorEnum *>(other)->getEnum();
//             break;
//         default:
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     for (const auto &value : values) {
//         if (!value.isFloat())
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//         auto dbl = value.getFloat();
//         if (dbl < m_minimum
//             || (m_minimum == dbl && m_minExclusive)
//             || dbl > m_maximum
//             || (m_maximum == dbl && m_maxExclusive))
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     return ValidatorComparisonType::COMPARE_SUBSET;
// }

tempo_config::ValidateString::ValidateString(
    tu_int32 minLength,
    tu_int32 maxLength,
    std::string_view pattern)
    : m_minLength(minLength),
      m_maxLength(maxLength),
      m_pattern(pattern)
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateString::getType() const
{
    return ValidatorNodeType::String;
}

tu_int32
tempo_config::ValidateString::getMinLength() const
{
    return m_minLength;
}

tu_int32
tempo_config::ValidateString::getMaxLength() const
{
    return m_maxLength;
}

std::string
tempo_config::ValidateString::getPattern() const
{
    return m_pattern;
}

tempo_utils::Status
tempo_config::ValidateString::validate(const ConfigNode &node)
{
    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a string", node.toString());
    auto value = node.toValue();
    auto str = value.getValue();

    if (str.size() < m_minLength)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "length of '{}' is less than {} code points", str, m_minLength);
    if (str.size() > m_maxLength)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "length of '{}' is greater than {} code points", str, m_maxLength);
    if (!m_pattern.empty() && !RE2::FullMatch(str, m_pattern))
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "'{}' does not match regex '{}'", str, m_pattern);

    return {};
}

// ValidatorComparisonType
// ValidateString::compare(ValidatorNode *other) const
// {
//     if (!other)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//
//     if (other->getType() != ValidatorNodeType::SCHEMA_STRING) {
//         auto *otherString = static_cast<ValidateString *>(other);
//
//         if (m_minLength == otherString->getMinLength() &&
//             m_maxLength == otherString->getMaxLength() &&
//             m_pattern == otherString->getPattern())
//             return ValidatorComparisonType::COMPARE_EQUAL;
//
//         if (m_minLength <= otherString->getMinLength() &&
//             m_maxLength >= otherString->getMaxLength() &&
//             m_pattern == otherString->getPattern())
//             return ValidatorComparisonType::COMPARE_SUBSET;
//     }
//
//     QSet<Atom> values;
//     switch (other->getType()) {
//         case ValidatorNodeType::SCHEMA_CONST:
//             values = {static_cast<ValidatorConst *>(other)->getConst()};
//             break;
//         case ValidatorNodeType::SCHEMA_ENUM:
//             values = static_cast<ValidatorEnum *>(other)->getEnum();
//             break;
//         default:
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     for (const auto &value : values) {
//         if (!value.isString())
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//         auto string = value.getString();
//         if (string.size() < m_minLength || m_maxLength < string.size())
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     return ValidatorComparisonType::COMPARE_SUBSET;
// }

tempo_config::ValidateSeq::ValidateSeq(
    tu_uint32 minItems,
    tu_uint32 maxItems,
    bool unique,
    std::shared_ptr<ValidatorNode> items,
    std::shared_ptr<ValidatorNode> contains)
    : m_minItems(minItems),
      m_maxItems(maxItems),
      m_unique(unique),
      m_items(std::move(items)),
      m_contains(std::move(contains))
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateSeq::getType() const
{
    return ValidatorNodeType::Seq;
}

tu_uint32
tempo_config::ValidateSeq::getMinItems() const
{
    return m_minItems;
}

tu_uint32
tempo_config::ValidateSeq::getMaxItems() const
{
    return m_maxItems;
}

bool
tempo_config::ValidateSeq::getUnique() const
{
    return m_unique;
}

std::shared_ptr<tempo_config::ValidatorNode>
tempo_config::ValidateSeq::getItems() const
{
    return m_items;
}

std::shared_ptr<tempo_config::ValidatorNode>
tempo_config::ValidateSeq::getContains() const
{
    return m_contains;
}

tempo_utils::Status
tempo_config::ValidateSeq::validate(const ConfigNode &node)
{
    if (node.getNodeType() != ConfigNodeType::kSeq)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a seq", node.toString());
    auto seq = node.toSeq();

    if (seq.seqSize() < m_minItems)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} contains less than {} items", node.toString(), m_minItems);
    if (seq.seqSize() > m_maxItems)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} contains more than {} items", node.toString(), m_maxItems);

    bool contains = m_contains ? false : true;
    absl::flat_hash_set<ConfigNode> uniqueItems;
    std::vector<std::string> errors;

    for (int i = 0; i < seq.seqSize(); i++) {
        const auto &item = seq.seqAt(i);

        if (m_items) {
            auto status = m_items->validate(item);
            if (status.notOk()) {
                errors.push_back(fmt::format(
                    "element {} does not conform to 'items' schema: {}", i, status.getMessage()));
            }
        }
        if (!contains) {
            auto status = m_contains->validate(item);
            if (status.isOk()) {
                contains = true;
            }
        }
        if (m_unique) {
            if (uniqueItems.contains(item)) {
                errors.push_back(fmt::format("element {} is not unique", i));
            }
        }
    }

    if (!contains) {
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} has no element conforming to 'contains' schema", node.toString());
    }

    return {};
}

tempo_config::ValidateMap::ValidateMap(
    tu_uint32 minProperties,
    tu_uint32 maxProperties,
    const absl::flat_hash_set<std::string> &required,
    const absl::flat_hash_map<std::string,std::shared_ptr<ValidatorNode>> &properties,
    std::shared_ptr<ValidatorNode> additional)
    : m_minProperties(minProperties),
      m_maxProperties(maxProperties),
      m_required(required),
      m_properties(properties),
      m_additional(std::move(additional))
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateMap::getType() const
{
    return ValidatorNodeType::Map;
}

tu_uint32
tempo_config::ValidateMap::getMinProperties() const
{
    return m_minProperties;
}

tu_uint32
tempo_config::ValidateMap::getMaxProperties() const
{
    return m_maxProperties;
}

bool
tempo_config::ValidateMap::hasRequired(std::string_view name) const
{
    return m_required.contains(name);
}

absl::flat_hash_set<std::string>::const_iterator
tempo_config::ValidateMap::requiredBegin() const
{
    return m_required.cbegin();
}

absl::flat_hash_set<std::string>::const_iterator
tempo_config::ValidateMap::requiredEnd() const
{
    return m_required.cend();
}

int
tempo_config::ValidateMap::numRequired() const
{
    return m_required.size();
}

bool
tempo_config::ValidateMap::hasProperty(std::string_view name) const
{
    return m_properties.contains(name);
}

std::shared_ptr<tempo_config::ValidatorNode>
tempo_config::ValidateMap::getProperty(std::string_view name) const
{
    auto entry = m_properties.find(name);
    if (entry != m_properties.cend())
        return entry->second;
    return {};
}

absl::flat_hash_map<std::string,std::shared_ptr<tempo_config::ValidatorNode>>::const_iterator
tempo_config::ValidateMap::propertiesBegin() const
{
    return m_properties.cbegin();
}

absl::flat_hash_map<std::string,std::shared_ptr<tempo_config::ValidatorNode>>::const_iterator
tempo_config::ValidateMap::propertiesEnd() const
{
    return m_properties.cend();
}

int
tempo_config::ValidateMap::numProperties() const
{
    return m_properties.size();
}

tempo_utils::Status
tempo_config::ValidateMap::validate(const ConfigNode &node)
{
    if (node.getNodeType() != ConfigNodeType::kMap)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is not a map", node.toString());
    auto map = node.toMap();

    if (map.mapSize() < m_minProperties)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} contains less than {} properties", node.toString(), m_minProperties);
    if (map.mapSize() > m_maxProperties)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} contains more than {} properties", node.toString(), m_maxProperties);

    std::vector<std::string> missing;
    for (const auto &name : m_required) {
        if (!map.mapContains(name)) {
            auto propName = absl::StrCat("\"", name, "\"");
            missing.push_back(std::move(propName));
        }
    }
    if (!missing.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "{} is missing required properties: ", node.toString(), absl::StrJoin(missing, ", "));

    std::vector<std::string> errors;

    for (auto it = map.mapBegin(); it != map.mapEnd(); ++it) {
        const auto &name = it->first;
        const auto &value = it->second;

        auto pentry = m_properties.find(name);
        if (pentry != m_properties.cend()) {
            auto property = pentry->second;
            auto status = property->validate(value);
            if (status.notOk()) {
                errors.push_back(fmt::format(
                    "property '{}' does not conform to 'properties' schema: {}", name, status.getMessage()));
            }
        } else if (m_additional != nullptr) {
            auto status = m_additional->validate(value);
            if (status.notOk()) {
                errors.push_back(fmt::format(
                    "property '{}' does not conform to 'additional' schema: {}", name, status.getMessage()));
            }
        }
    }

    return {};
}

tempo_config::ValidateAnyOf::ValidateAnyOf() : ValidatorNode()
{
}

tempo_config::ValidateAnyOf::ValidateAnyOf(const std::vector<std::shared_ptr<ValidatorNode>> &anyOf)
    : m_anyOf(anyOf)
{
}

tempo_config::ValidateAnyOf::ValidateAnyOf(std::vector<std::shared_ptr<ValidatorNode>> &&anyOf)
    : m_anyOf(std::move(anyOf))
{
}

tempo_config::ValidateAnyOf::ValidateAnyOf(std::initializer_list<std::shared_ptr<ValidatorNode>> anyOf)
    : m_anyOf(anyOf.begin(), anyOf.end())
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateAnyOf::getType() const
{
    return ValidatorNodeType::AnyOf;
}

std::shared_ptr<tempo_config::ValidatorNode>
tempo_config::ValidateAnyOf::getNode(int index) const
{
    if (index < 0 || m_anyOf.size() <= index)
        return {};
    return m_anyOf.at(index);
}

std::vector<std::shared_ptr<tempo_config::ValidatorNode>>::const_iterator
tempo_config::ValidateAnyOf::cbegin() const
{
    return m_anyOf.cbegin();
}

std::vector<std::shared_ptr<tempo_config::ValidatorNode>>::const_iterator
tempo_config::ValidateAnyOf::cend() const
{
    return m_anyOf.cend();
}

int
tempo_config::ValidateAnyOf::numNodes() const
{
    return m_anyOf.size();
}

tempo_utils::Status
tempo_config::ValidateAnyOf::validate(const ConfigNode &node)
{
    std::vector<std::string> errors;

    for (auto &schema : m_anyOf) {
        auto status = schema->validate(node);
        if (status.isOk())
            return {};
        errors.emplace_back(status.getMessage());
    }
    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "{} does not conform to any schema: [ {} ]", node.toString(),
        absl::StrJoin(errors, ", "));
}

tempo_config::ValidateAllOf::ValidateAllOf()
{
}

tempo_config::ValidateAllOf::ValidateAllOf(const std::vector<std::shared_ptr<ValidatorNode>> &allOf)
    : m_allOf(allOf)
{
}

tempo_config::ValidateAllOf::ValidateAllOf(std::vector<std::shared_ptr<ValidatorNode>> &&allOf)
    : m_allOf(std::move(allOf))
{
}

tempo_config::ValidateAllOf::ValidateAllOf(std::initializer_list<std::shared_ptr<ValidatorNode>> allOf)
    : m_allOf(allOf.begin(), allOf.end())
{
}

tempo_config::ValidatorNodeType
tempo_config::ValidateAllOf::getType() const
{
    return ValidatorNodeType::AllOf;
}

std::shared_ptr<tempo_config::ValidatorNode>
tempo_config::ValidateAllOf::getNode(int index) const
{
    if (index < 0 || m_allOf.size() <= index)
        return {};
    return m_allOf.at(index);
}

std::vector<std::shared_ptr<tempo_config::ValidatorNode>>::const_iterator
tempo_config::ValidateAllOf::cbegin() const
{
    return m_allOf.cbegin();
}

std::vector<std::shared_ptr<tempo_config::ValidatorNode>>::const_iterator
tempo_config::ValidateAllOf::cend() const
{
    return m_allOf.cend();
}

int
tempo_config::ValidateAllOf::numNodes() const
{
    return m_allOf.size();
}

tempo_utils::Status
tempo_config::ValidateAllOf::validate(const ConfigNode &node)
{
    std::vector<std::string> errors;

    for (auto &schema : m_allOf) {
        auto status = schema->validate(node);
        if (status.notOk()) {
            errors.emplace_back(status.getMessage());
        }
    }
    if (errors.empty())
        return {};
    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "{} does not conform to any schema: [ {} ]", node.toString(),
        absl::StrJoin(errors, ", "));
}

tempo_config::ValidateNot::ValidateNot()
{
}

tempo_config::ValidateNot::ValidateNot(std::shared_ptr<ValidatorNode> node)
    : m_node(std::move(node))
{
    TU_ASSERT (m_node != nullptr);
}

tempo_config::ValidatorNodeType
tempo_config::ValidateNot::getType() const
{
    return ValidatorNodeType::Not;
}

std::shared_ptr<tempo_config::ValidatorNode>
tempo_config::ValidateNot::getNode() const
{
    return m_node;
}

tempo_utils::Status
tempo_config::ValidateNot::validate(const ConfigNode &node)
{
    if (m_node != nullptr) {
        auto status = m_node->validate(node);
        if (status.isOk())
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "{} matches negation", node.toString());
    }
    return {};
}