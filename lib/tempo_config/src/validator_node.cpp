
#include <absl/strings/str_join.h>
#include <re2/re2.h>

#include <tempo_config/config_result.h>
#include <tempo_config/validator_node.h>
#include <tempo_utils/logging.h>

tempo_config::ValidatorNodeType
tempo_config::ValidateAny::getType() const
{
    return ValidatorNodeType::SCHEMA_ANY;
}

tempo_config::ValidatorValueType
tempo_config::ValidateAny::getValueType() const
{
    return ValidatorValueType::VALUE_ANY;
}

tempo_utils::Status
tempo_config::ValidateAny::validate(const ConfigNode &node)
{
    return {};
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
    return ValidatorNodeType::SCHEMA_INTEGER;
}

tempo_config::ValidatorValueType
tempo_config::ValidateInteger::getValueType() const
{
    return ValidatorValueType::VALUE_INTEGER;
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
    return ValidatorNodeType::SCHEMA_FLOAT;
}

tempo_config::ValidatorValueType
tempo_config::ValidateFloat::getValueType() const
{
    return ValidatorValueType::VALUE_FLOAT;
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
    return ValidatorNodeType::SCHEMA_STRING;
}

tempo_config::ValidatorValueType
tempo_config::ValidateString::getValueType() const
{
    return ValidatorValueType::VALUE_STRING;
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

// ValidatorSeq::ValidatorSeq()
// {
//     m_minItems = 0;
//     m_maxItems = std::numeric_limits<qint64>::max();
//     m_items = nullptr;
//     m_uniqueItems = false;
//     m_contains = nullptr;
// }
//
// ValidatorSeq::~ValidatorSeq()
// {
//     delete m_items;
//     delete m_contains;
// }
//
// ValidatorNodeType
// ValidatorSeq::getType() const
// {
//     return ValidatorNodeType::SCHEMA_SEQ;
// }
//
// ValidatorValueType
// ValidatorSeq::getValueType() const
// {
//     return ValidatorValueType::VALUE_SEQ;
// }
//
// qint64
// ValidatorSeq::getMinItems() const
// {
//     return m_minItems;
// }
//
// void
// ValidatorSeq::setMinItems(qint64 minItems)
// {
//     m_minItems = minItems;
// }
//
// qint64
// ValidatorSeq::getMaxItems() const
// {
//     return m_maxItems;
// }
//
// void
// ValidatorSeq::setMaxItems(qint64 maxItems)
// {
//     m_maxItems = maxItems;
// }
//
// ValidatorNode *
// ValidatorSeq::getItems() const
// {
//     return m_items;
// }
//
// void
// ValidatorSeq::setItems(ValidatorNode *node)
// {
//     Q_ASSERT(node != nullptr);
//     delete m_items;
//     m_items = node;
//     node->setParent(this);
// }
//
// bool
// ValidatorSeq::getUniqueItems() const
// {
//     return m_uniqueItems;
// }
//
// void
// ValidatorSeq::setUniqueItems(bool uniqueItems)
// {
//     m_uniqueItems = uniqueItems;
// }
//
// ValidatorNode *
// ValidatorSeq::getContains() const
// {
//     return m_contains;
// }
//
// void
// ValidatorSeq::setContains(ValidatorNode *node)
// {
//     Q_ASSERT(node != nullptr);
//     delete m_contains;
//     m_contains = node;
//     node->setParent(this);
// }
//
// SideEffect
// ValidatorSeq::validate(const Atom &atom)
// {
//     if (!atom.isSeq())
//         return SideEffect::failure(RuntimeError(QString("%1 is not a seq").arg(atom.toString())));
//     if (atom.seqSize() < m_minItems)
//         return SideEffect::failure(RuntimeError(QString("%1 item count is less than %2").arg(atom.toString()).arg(m_minItems)));
//     if (atom.seqSize() > m_maxItems)
//         return SideEffect::failure(RuntimeError(QString("%1 item count is greater than %2").arg(atom.toString()).arg(m_maxItems)));
//
//     bool contains = m_contains ? false : true;
//     for (auto iterator = atom.seqBegin(); iterator != atom.seqEnd(); iterator++) {
//         auto &item = *iterator;
//
//         if (m_items) {
//             auto result = m_items->validate(item);
//             if (result.isFailure())
//                 return SideEffect::failure(RuntimeError(QString("%1 does not conform to schema: %2").arg(item.toString()).arg(result.getError().getMessage())));
//         }
//         if (m_contains && !contains && m_contains->validate(item).isSuccess())
//             contains = true;
//     }
//
//     if (!contains)
//         return SideEffect::failure(RuntimeError(QString("%1 does not contain item conforming to schema").arg(atom.toString())));
//
//     return SideEffect::success();
//
// }
//
// ValidatorComparisonType
// ValidatorSeq::compare(ValidatorNode *other) const
// {
//     return ValidatorComparisonType::COMPARE_DISJOINT;
// }
//
// uint
// ValidatorSeq::hash() const
// {
//     auto result = qHash(m_minItems) + qHash(m_maxItems) + qHash(m_uniqueItems);
//     if (m_items)
//         result += m_items->hash();
//     if (m_contains)
//         result += m_contains->hash();
//     return result;
// }
//
// ValidatorNode *
// ValidatorSeq::clone() const
// {
//     ValidatorSeq *array = new ValidatorSeq;
//     array->m_minItems = m_minItems;
//     array->m_maxItems = m_maxItems;
//     array->m_uniqueItems = m_uniqueItems;
//     if (m_items)
//         array->m_items = m_items->clone();
//     if (m_contains)
//         array->m_contains = m_contains->clone();
//     return array;
// }
//
// ValidatorMap::ValidatorMap() : ValidatorNode()
// {
//     m_minProperties = 0;
//     m_maxProperties = std::numeric_limits<qint64>::max();
// }
//
// ValidatorMap::~ValidatorMap()
// {
//     for (auto iterator = m_properties.begin(); iterator != m_properties.end(); iterator++) {
//         delete iterator.value();
//     }
//     m_properties.clear();
// }
//
// ValidatorNodeType
// ValidatorMap::getType() const
// {
//     return ValidatorNodeType::SCHEMA_MAP;
// }
//
// ValidatorValueType
// ValidatorMap::getValueType() const
// {
//     return ValidatorValueType::VALUE_MAP;
// }
//
// qint64
// ValidatorMap::getMinProperties() const
// {
//     return m_minProperties;
// }
//
// void
// ValidatorMap::setMinProperties(qint64 minProperties)
// {
//     m_minProperties = minProperties;
// }
//
// qint64
// ValidatorMap::getMaxProperties() const
// {
//     return m_maxProperties;
// }
//
// void
// ValidatorMap::setMaxProperties(qint64 maxProperties)
// {
//     m_maxProperties = maxProperties;
// }
//
// QStringList
// ValidatorMap::getRequired() const
// {
//     return QList<QString>(m_required.cbegin(), m_required.cend());
// }
//
// void
// ValidatorMap::setRequired(const QStringList &required)
// {
//     m_required = QSet<QString>(required.cbegin(), required.cend());
// }
//
// ValidatorNode *
// ValidatorMap::getProperty(const QString &name) const
// {
//     if (!m_properties.contains(name))
//         return nullptr;
//     return m_properties[name];
// }
//
// void
// ValidatorMap::setProperty(const QString &name, ValidatorNode *node)
// {
//     Q_ASSERT(node != nullptr);
//     removeProperty(name);
//     m_properties[name] = node;
//     node->setParent(this);
// }
//
// void
// ValidatorMap::removeProperty(const QString &name)
// {
//     if (m_properties.contains(name))
//         delete m_properties.take(name);
// }
//
// int
// ValidatorMap::numProperties() const
// {
//     return m_properties.size();
// }
//
// SideEffect
// ValidatorMap::validate(const Atom &atom)
// {
//     if (!atom.isMap())
//         return SideEffect::failure(RuntimeError(QString("%1 is not a map").arg(atom.toString())));
//     if (atom.mapSize() < m_minProperties)
//         return SideEffect::failure(RuntimeError(QString("%1 property count is less than %2").arg(atom.toString()).arg(m_minProperties)));
//     if (atom.mapSize() > m_maxProperties)
//         return SideEffect::failure(RuntimeError(QString("%1 property count is greater than %2").arg(atom.toString()).arg(m_maxProperties)));
//     if (!m_required.isEmpty()) {
//         auto missingKeys = QSet<QString>(atom.mapKeys().cbegin(), atom.mapKeys().cend());
//         missingKeys.subtract(m_required);
//         if (!missingKeys.isEmpty()) {
//             return SideEffect::failure(
//                 RuntimeError(QString("%1 is missing required keys %2")
//                 .arg(atom.toString())
//                 .arg(QStringList(missingKeys.cbegin(), missingKeys.cend()).join(", "))));
//         }
//     }
//     for (auto iterator = m_properties.cbegin(); iterator != m_properties.cend(); iterator++) {
//         auto name = iterator.key();
//         auto schema = iterator.value();
//         if (!atom.mapContains(name))
//             return SideEffect::failure(RuntimeError(QString("%1 is missing required property %2").arg(atom.toString()).arg(name)));
//         auto result = schema->validate(atom.mapAt(name));
//         if (result.isFailure())
//             return result;
//     }
//     return SideEffect::success();
// }
//
// ValidatorComparisonType
// ValidatorMap::compare(ValidatorNode *other) const
// {
//     return ValidatorComparisonType::COMPARE_DISJOINT;
// }
//
// uint
// ValidatorMap::hash() const
// {
//     auto result = qHash(m_minProperties) + qHash(m_maxProperties) + qHash(m_required);
//     auto propertyKeys = m_properties.keys();
//     std::sort(propertyKeys.begin(), propertyKeys.end());
//     for (const auto &key : propertyKeys) {
//         result += qHash(key);
//         result += m_properties[key]->hash();
//     }
//     return result;
// }
//
// ValidatorNode *
// ValidatorMap::clone() const
// {
//     auto object = new ValidatorMap();
//     object->m_minProperties = m_minProperties;
//     object->m_maxProperties = m_maxProperties;
//     object->m_required = m_required;
//     for (auto iterator = m_properties.cbegin(); iterator != m_properties.cend(); iterator++) {
//         object->m_properties[iterator.key()] = iterator.value()->clone();
//     }
//     return object;
// }
//
// ValidatorClass::ValidatorClass()
// {
// }
//
// ValidatorNodeType
// ValidatorClass::getType() const
// {
//     return ValidatorNodeType::SCHEMA_CLASS;
// }
//
// ValidatorValueType
// ValidatorClass::getValueType() const
// {
//     return ValidatorValueType::VALUE_CLASS;
// }
//
// QVector<uint16_t>
// ValidatorClass::getAncestry() const
// {
//     return m_ancestry;
// }
//
// void
// ValidatorClass::setAncestry(const QVector<uint16_t> &ancestry)
// {
//     m_ancestry = ancestry;
// }
//
// SideEffect
// ValidatorClass::validate(const Atom &atom)
// {
//     return SideEffect::failure(RuntimeError(QString("%1 is not an object").arg(atom.toString())));
// }
//
// ValidatorComparisonType
// ValidatorClass::compare(ValidatorNode *other) const
// {
//     if (!other)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//
//     if (other->getType() != ValidatorNodeType::SCHEMA_CLASS)
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//
//     auto otherAncestry = static_cast<ValidatorClass *>(other)->getAncestry();
//     if (otherAncestry.size() < m_ancestry.size())
//         return ValidatorComparisonType::COMPARE_DISJOINT;
//
//     for (int i = 0; i < m_ancestry.size(); i++) {
//         if (m_ancestry[i] != otherAncestry[i])
//             return ValidatorComparisonType::COMPARE_DISJOINT;
//     }
//
//     if (otherAncestry.size() == m_ancestry.size())
//         return ValidatorComparisonType::COMPARE_EQUAL;
//     return ValidatorComparisonType::COMPARE_SUBSET;
// }
//
// uint
// ValidatorClass::hash() const
// {
//     return qHash(m_ancestry);
// }
//
// ValidatorNode *
// ValidatorClass::clone() const
// {
//     auto object = new ValidatorClass();
//     object->m_ancestry = m_ancestry;
//     return object;
// }

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
    return ValidatorNodeType::SCHEMA_ANY_OF;
}

tempo_config::ValidatorValueType
tempo_config::ValidateAnyOf::getValueType() const
{
    if (m_anyOf.empty())
        return ValidatorValueType::VALUE_ANY;

    auto valueType = m_anyOf.front()->getValueType();
    if (m_anyOf.size() == 1)
        return valueType;

    for (int i = 1; i < m_anyOf.size(); i++) {
        auto otherType = m_anyOf[i]->getValueType();
        if (otherType != valueType)
            return ValidatorValueType::VALUE_ANY_OF;
    }
    return valueType;
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
    return ValidatorNodeType::SCHEMA_ALL_OF;
}

tempo_config::ValidatorValueType
tempo_config::ValidateAllOf::getValueType() const
{
    if (m_allOf.empty())
        return ValidatorValueType::VALUE_ANY;

    auto valueType = m_allOf.front()->getValueType();
    if (m_allOf.size() == 1)
        return valueType;

    for (int i = 1; i < m_allOf.size(); i++) {
        auto otherType = m_allOf[i]->getValueType();
        if (otherType != valueType)
            return ValidatorValueType::VALUE_ALL_OF;
    }
    return valueType;
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
    return ValidatorNodeType::SCHEMA_NOT;
}

tempo_config::ValidatorValueType
tempo_config::ValidateNot::getValueType() const
{
    if (m_node == nullptr)
        return ValidatorValueType::VALUE_ANY;
    return ValidatorValueType::VALUE_NOT;
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

// QDebug
// operator<<(QDebug dbg, const ValidatorNodeType &t)
// {
//     switch (t) {
//         case ValidatorNodeType::SCHEMA_EMPTY:
//             dbg.noquote() << "EMPTY";
//             break;
//         case ValidatorNodeType::SCHEMA_ANY:
//             dbg.noquote() << "ANY";
//             break;
//         case ValidatorNodeType::SCHEMA_CONST:
//             dbg.noquote() << "CONST";
//             break;
//         case ValidatorNodeType::SCHEMA_ENUM:
//             dbg.noquote() << "ENUM";
//             break;
//         case ValidatorNodeType::SCHEMA_INTEGER:
//             dbg.noquote() << "INTEGER";
//             break;
//         case ValidatorNodeType::SCHEMA_FLOAT:
//             dbg.noquote() << "FLOAT";
//             break;
//         case ValidatorNodeType::SCHEMA_STRING:
//             dbg.noquote() << "STRING";
//             break;
//         case ValidatorNodeType::SCHEMA_SEQ:
//             dbg.noquote() << "SEQ";
//             break;
//         case ValidatorNodeType::SCHEMA_MAP:
//             dbg.noquote() << "MAP";
//             break;
//         case ValidatorNodeType::SCHEMA_CLASS:
//             dbg.noquote() << "CLASS";
//             break;
//         case ValidatorNodeType::SCHEMA_ANY_OF:
//             dbg.noquote() << "ANY_OF";
//             break;
//         case ValidatorNodeType::SCHEMA_ALL_OF:
//             dbg.noquote() << "ALL_OF";
//             break;
//         case ValidatorNodeType::SCHEMA_NOT:
//             dbg.noquote() << "NOT";
//             break;
//     }
//     return dbg;
// }
