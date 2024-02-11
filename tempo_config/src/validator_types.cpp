//#include <QSet>
//
//#include "logging.h"
//
//#include "schema_node.h"
//
//DECLARE_LOGGER("SchemaNode")
//
//SchemaNode::SchemaNode()
//{
//    m_parent = nullptr;;
//}
//
//SchemaNode *
//SchemaNode::getParent() const
//{
//    return m_parent;
//}
//
//void
//SchemaNode::setParent(SchemaNode *parent)
//{
//    m_parent = parent;
//}
//
//SchemaNodeType
//SchemaAny::getType() const
//{
//    return SchemaNodeType::SCHEMA_ANY;
//}
//
//SchemaValueType
//SchemaAny::getValueType() const
//{
//    return SchemaValueType::VALUE_ANY;
//}
//
//SideEffect
//SchemaAny::validate(const Atom &atom)
//{
//    return SideEffect::success();
//}
//
//SchemaComparisonType
//SchemaAny::compare(SchemaNode *other) const
//{
//    if (!other || other->getType() == SchemaNodeType::SCHEMA_EMPTY)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//    if (other->getType() == SchemaNodeType::SCHEMA_ANY)
//        return SchemaComparisonType::COMPARE_EQUAL;
//    return SchemaComparisonType::COMPARE_SUBSET;
//}
//
//uint
//SchemaAny::hash() const
//{
//    return 0;
//}
//
//SchemaNode *
//SchemaAny::clone() const
//{
//    return new SchemaAny();
//}
//
//SchemaConst::SchemaConst() : SchemaNode()
//{
//}
//
//SchemaNodeType
//SchemaConst::getType() const
//{
//    return SchemaNodeType::SCHEMA_CONST;
//}
//
//SchemaValueType
//atom_type_to_value_type(AtomType atomType)
//{
//    switch (atomType) {
//        case AtomType::NULL_TYPE:
//            return SchemaValueType::VALUE_NULL;
//        case AtomType::BOOLEAN_TYPE:
//            return SchemaValueType::VALUE_BOOL;
//        case AtomType::INTEGER_TYPE:
//            return SchemaValueType::VALUE_INTEGER;
//        case AtomType::FLOAT_TYPE:
//            return SchemaValueType::VALUE_FLOAT;
//        case AtomType::STRING_TYPE:
//            return SchemaValueType::VALUE_STRING;
//        case AtomType::SEQ_TYPE:
//            return SchemaValueType::VALUE_SEQ;
//        case AtomType::MAP_TYPE:
//            return SchemaValueType::VALUE_MAP;
//        default:
//            break;
//    }
//    return SchemaValueType::VALUE_EMPTY;
//}
//
//SchemaValueType
//SchemaConst::getValueType() const
//{
//    return atom_type_to_value_type(m_const.getType());
//}
//
//Atom
//SchemaConst::getConst() const
//{
//    return m_const;
//}
//
//void
//SchemaConst::setConst(const Atom &const_)
//{
//    m_const = const_;
//}
//
//SideEffect
//SchemaConst::validate(const Atom &atom)
//{
//    if (atom == m_const)
//        return SideEffect::success();
//    return SideEffect::failure(RuntimeError(QString("%1 is not const %2").arg(atom.toString()).arg(m_const.toString())));
//}
//
//SchemaComparisonType
//SchemaConst::compare(SchemaNode *other) const
//{
//    if (!other)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//    if (other->getType() == SchemaNodeType::SCHEMA_CONST && m_const == static_cast<SchemaConst *>(other)->getConst())
//        return SchemaComparisonType::COMPARE_EQUAL;
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaConst::hash() const
//{
//    return qHash(m_const);
//}
//
//SchemaNode *
//SchemaConst::clone() const
//{
//    auto const_ = new SchemaConst();
//    const_->m_const = m_const;
//    return const_;
//}
//
//SchemaEnum::SchemaEnum() : SchemaNode()
//{
//}
//
//SchemaNodeType
//SchemaEnum::getType() const
//{
//    return SchemaNodeType::SCHEMA_ENUM;
//}
//
//SchemaValueType
//SchemaEnum::getValueType() const
//{
//    if (m_enum.isEmpty())
//        return SchemaValueType::VALUE_EMPTY;
//    //auto enum_ = m_enum.toList();
//    auto enum_ = QList<Atom>(m_enum.cbegin(), m_enum.cend());
//    SchemaValueType valueType = atom_type_to_value_type(enum_.first().getType());
//    if (m_enum.size() == 1)
//        return valueType;
//    for (int i = 1; i < m_enum.size(); i++) {
//        SchemaValueType otherType = atom_type_to_value_type(enum_[i].getType());
//        if (otherType != valueType)
//            return SchemaValueType::VALUE_ANY;
//    }
//    return valueType;
//}
//
//QSet<Atom>
//SchemaEnum::getEnum() const
//{
//    return m_enum;
//}
//
//void
//SchemaEnum::setEnum(const QSet<Atom> &enum_)
//{
//    m_enum = enum_;
//}
//
//SideEffect
//SchemaEnum::validate(const Atom &atom)
//{
//    if (m_enum.contains(atom))
//        return SideEffect::success();
//    QStringList sl;
//    for (auto iterator = m_enum.cbegin(); iterator != m_enum.cend(); iterator++) {
//        sl.append(iterator->toString());
//    }
//    return SideEffect::failure(RuntimeError(QString("%1 not in enum [%2]").arg(atom.toString()).arg(sl.join(','))));
//}
//
//SchemaComparisonType
//SchemaEnum::compare(SchemaNode *other) const
//{
//    if (!other)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//    if (other->getType() == SchemaNodeType::SCHEMA_CONST) {
//        auto c = static_cast<SchemaConst *>(other)->getConst();
//        if (m_enum.contains(c))
//            return SchemaComparisonType::COMPARE_SUBSET;
//    }
//    if (other->getType() == SchemaNodeType::SCHEMA_ENUM) {
//        if (m_enum == static_cast<SchemaEnum *>(other)->getEnum())
//            return SchemaComparisonType::COMPARE_EQUAL;
//    }
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaEnum::hash() const
//{
//    uint result = 0;
//    for (const auto &e : m_enum) {
//        result += qHash(e);
//    }
//    return result;
//}
//
//SchemaNode *
//SchemaEnum::clone() const
//{
//    auto enum_ = new SchemaEnum();
//    enum_->m_enum = m_enum;
//    return enum_;
//}
//
//SchemaInteger::SchemaInteger() : SchemaNode()
//{
//    m_multipleOf = 1;
//    m_minimum = std::numeric_limits<qint64>::min();
//    m_minExclusive = false;
//    m_maximum = std::numeric_limits<qint64>::max();
//    m_maxExclusive = false;
//}
//
//SchemaNodeType
//SchemaInteger::getType() const
//{
//    return SchemaNodeType::SCHEMA_INTEGER;
//}
//
//SchemaValueType
//SchemaInteger::getValueType() const
//{
//    return SchemaValueType::VALUE_INTEGER;
//}
//
//qint64
//SchemaInteger::getMultipleOf() const
//{
//    return m_multipleOf;
//}
//
//void
//SchemaInteger::setMultipleOf(qint64 multipleOf)
//{
//    m_multipleOf = multipleOf;
//}
//
//qint64
//SchemaInteger::getMinimum() const
//{
//    return m_minimum;
//}
//
//void
//SchemaInteger::setMinimum(qint64 minimum)
//{
//    m_minimum = minimum;
//}
//
//bool
//SchemaInteger::getMinExclusive() const
//{
//    return m_minExclusive;
//}
//
//void
//SchemaInteger::setMinExclusive(bool exclusive)
//{
//    m_minExclusive = exclusive;
//}
//
//qint64
//SchemaInteger::getMaximum() const
//{
//    return m_maximum;
//}
//
//void
//SchemaInteger::setMaximum(qint64 maximum)
//{
//    m_maximum = maximum;
//}
//
//bool
//SchemaInteger::getMaxExclusive() const
//{
//    return m_maxExclusive;
//}
//
//void
//SchemaInteger::setMaxExclusive(bool exclusive)
//{
//    m_maxExclusive = exclusive;
//}
//
//SideEffect
//SchemaInteger::validate(const Atom &atom)
//{
//    if (!atom.isInteger())
//        return SideEffect::failure(RuntimeError(QString("%1 is not an integer").arg(atom.toString())));
//    auto result = atom.convertLong();
//    if (result.isFailure())
//        return SideEffect::failure(result.getError());
//    auto i64 = result.getResult();
//    if (i64 < m_minimum)
//        return SideEffect::failure(RuntimeError(QString("%1 is less than %2").arg(atom.toString()).arg(m_minimum)));
//    if (m_minExclusive && i64 == m_minimum)
//        return SideEffect::failure(RuntimeError(QString("%1 is equal to %2").arg(atom.toString()).arg(m_minimum)));
//    if (i64 > m_maximum)
//        return SideEffect::failure(RuntimeError(QString("%1 is greater than %2").arg(atom.toString()).arg(m_maximum)));
//    if (m_maxExclusive && i64 == m_maximum)
//        return SideEffect::failure(RuntimeError(QString("%1 is equal to %2").arg(atom.toString()).arg(m_maximum)));
//    if (i64 % m_multipleOf != 0)
//        return SideEffect::failure(RuntimeError(QString("%1 is not a multiple of %2").arg(atom.toString()).arg(m_multipleOf)));
//
//    return SideEffect::success();
//}
//
//SchemaComparisonType
//SchemaInteger::compare(SchemaNode *other) const
//{
//    if (!other)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//
//    if (other->getType() == SchemaNodeType::SCHEMA_INTEGER) {
//        auto *otherInteger = static_cast<SchemaInteger *>(other);
//
//        if (m_minimum == otherInteger->getMinimum() &&
//            m_minExclusive == otherInteger->getMinExclusive() &&
//            m_maximum == otherInteger->getMaximum() &&
//            m_maxExclusive == otherInteger->getMaxExclusive() &&
//            m_multipleOf == otherInteger->getMultipleOf())
//            return SchemaComparisonType::COMPARE_EQUAL;
//
//        if ((m_minimum <= otherInteger->getMinimum()
//            || (m_minimum == otherInteger->getMinimum() && otherInteger->getMinExclusive()))
//            && (m_maximum >= otherInteger->getMaximum()
//            || (m_maximum == otherInteger->getMaximum() && otherInteger->getMaxExclusive()))
//            && m_multipleOf == otherInteger->getMultipleOf())
//            return SchemaComparisonType::COMPARE_SUBSET;
//
//        return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    QSet<Atom> values;
//    switch (other->getType()) {
//        case SchemaNodeType::SCHEMA_CONST:
//            values = {static_cast<SchemaConst *>(other)->getConst()};
//            break;
//        case SchemaNodeType::SCHEMA_ENUM:
//            values = static_cast<SchemaEnum *>(other)->getEnum();
//            break;
//        default:
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    for (const auto &value : values) {
//        if (!value.isInteger())
//            return SchemaComparisonType::COMPARE_DISJOINT;
//        auto i64 = value.getInteger();
//        if (i64 < m_minimum
//            || (m_minimum == i64 && m_minExclusive)
//            || i64 > m_maximum
//            || (m_maximum == i64 && m_maxExclusive)
//            || (i64 % m_multipleOf != 0))
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    return SchemaComparisonType::COMPARE_SUBSET;
//}
//
//uint
//SchemaInteger::hash() const
//{
//    return qHash(m_minimum)
//        + qHash(m_maximum)
//        + qHash(m_minExclusive)
//        + qHash(m_maxExclusive)
//        + qHash(m_multipleOf);
//}
//
//SchemaNode *
//SchemaInteger::clone() const
//{
//    auto other = new SchemaInteger();
//    other->m_multipleOf = m_multipleOf;
//    other->m_minimum = m_minimum;
//    other->m_minExclusive = m_minExclusive;
//    other->m_maximum = m_maximum;
//    other->m_maxExclusive = m_maxExclusive;
//    return other;
//}
//
//SchemaFloat::SchemaFloat() : SchemaNode()
//{
//    m_minimum = std::numeric_limits<qint64>::min();
//    m_minExclusive = false;
//    m_maximum = std::numeric_limits<qint64>::max();
//    m_maxExclusive = false;
//}
//
//SchemaNodeType
//SchemaFloat::getType() const
//{
//    return SchemaNodeType::SCHEMA_FLOAT;
//}
//
//SchemaValueType
//SchemaFloat::getValueType() const
//{
//    return SchemaValueType::VALUE_FLOAT;
//}
//
//double
//SchemaFloat::getMinimum() const
//{
//    return m_minimum;
//}
//
//void
//SchemaFloat::setMinimum(double minimum)
//{
//    m_minimum = minimum;
//}
//
//bool
//SchemaFloat::getMinExclusive() const
//{
//    return m_minExclusive;
//}
//
//void
//SchemaFloat::setMinExclusive(bool exclusive)
//{
//    m_minExclusive = exclusive;
//}
//
//double
//SchemaFloat::getMaximum() const
//{
//    return m_maximum;
//}
//
//void
//SchemaFloat::setMaximum(double maximum)
//{
//    m_maximum = maximum;
//}
//
//bool
//SchemaFloat::getMaxExclusive() const
//{
//    return m_maxExclusive;
//}
//
//void
//SchemaFloat::setMaxExclusive(bool exclusive)
//{
//    m_maxExclusive = exclusive;
//}
//
//SideEffect
//SchemaFloat::validate(const Atom &atom)
//{
//    if (!atom.isFloat())
//        return SideEffect::failure(RuntimeError(QString("%1 is not a float").arg(atom.toString())));
//    auto result = atom.convertDouble();
//    if (result.isFailure())
//        return SideEffect::failure(result.getError());
//    auto dbl = result.getResult();
//    if (dbl < m_minimum)
//        return SideEffect::failure(RuntimeError(QString("%1 is less than %2").arg(atom.toString()).arg(m_minimum)));
//    if (m_minExclusive && dbl == m_minimum)
//        return SideEffect::failure(RuntimeError(QString("%1 is equal to %2").arg(atom.toString()).arg(m_minimum)));
//    if (dbl > m_maximum)
//        return SideEffect::failure(RuntimeError(QString("%1 is greater than %2").arg(atom.toString()).arg(m_maximum)));
//    if (m_maxExclusive && dbl == m_maximum)
//        return SideEffect::failure(RuntimeError(QString("%1 is equal to %2").arg(atom.toString()).arg(m_maximum)));
//
//    return SideEffect::success();
//}
//
//uint
//SchemaFloat::hash() const
//{
//    return qHash(m_minimum)
//        + qHash(m_maximum)
//        + qHash(m_minExclusive)
//        + qHash(m_maxExclusive);
//}
//
//
//SchemaComparisonType
//SchemaFloat::compare(SchemaNode *other) const
//{
//    if (!other)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//
//    if (other->getType() == SchemaNodeType::SCHEMA_FLOAT) {
//        auto *otherFloat = static_cast<SchemaFloat *>(other);
//
//        if (m_minimum == otherFloat->getMinimum() &&
//            m_minExclusive == otherFloat->getMinExclusive() &&
//            m_maximum == otherFloat->getMaximum() &&
//            m_maxExclusive == otherFloat->getMaxExclusive())
//            return SchemaComparisonType::COMPARE_EQUAL;
//
//        if ((m_minimum <= otherFloat->getMinimum()
//            || (m_minimum == otherFloat->getMinimum() && otherFloat->getMinExclusive()))
//            && (m_maximum >= otherFloat->getMaximum()
//            || (m_maximum == otherFloat->getMaximum() && otherFloat->getMaxExclusive())))
//            return SchemaComparisonType::COMPARE_SUBSET;
//
//        return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    QSet<Atom> values;
//    switch (other->getType()) {
//        case SchemaNodeType::SCHEMA_CONST:
//            values = {static_cast<SchemaConst *>(other)->getConst()};
//            break;
//        case SchemaNodeType::SCHEMA_ENUM:
//            values = static_cast<SchemaEnum *>(other)->getEnum();
//            break;
//        default:
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    for (const auto &value : values) {
//        if (!value.isFloat())
//            return SchemaComparisonType::COMPARE_DISJOINT;
//        auto dbl = value.getFloat();
//        if (dbl < m_minimum
//            || (m_minimum == dbl && m_minExclusive)
//            || dbl > m_maximum
//            || (m_maximum == dbl && m_maxExclusive))
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    return SchemaComparisonType::COMPARE_SUBSET;
//}
//
//SchemaNode *
//SchemaFloat::clone() const
//{
//    auto other = new SchemaFloat();
//    other->m_minimum = m_minimum;
//    other->m_minExclusive = m_minExclusive;
//    other->m_maximum = m_maximum;
//    other->m_maxExclusive = m_maxExclusive;
//    return other;
//}
//
//SchemaString::SchemaString() : SchemaNode()
//{
//    m_minLength = 0;
//    m_maxLength = std::numeric_limits<qint64>::max();
//}
//
//SchemaNodeType
//SchemaString::getType() const
//{
//    return SchemaNodeType::SCHEMA_STRING;
//}
//
//SchemaValueType
//SchemaString::getValueType() const
//{
//    return SchemaValueType::VALUE_STRING;
//}
//
//qint64
//SchemaString::getMinLength() const
//{
//    return m_minLength;
//}
//
//void
//SchemaString::setMinLength(qint64 minLength)
//{
//    m_minLength = minLength;
//}
//
//qint64
//SchemaString::getMaxLength() const
//{
//    return m_maxLength;
//}
//
//void
//SchemaString::setMaxLength(qint64 maxLength)
//{
//    m_maxLength = maxLength;
//}
//
//QString
//SchemaString::getPattern() const
//{
//    return m_pattern;
//}
//
//void
//SchemaString::setPattern(const QString &pattern)
//{
//    m_pattern = pattern;
//}
//
//SideEffect
//SchemaString::validate(const Atom &atom)
//{
//    if (!atom.isString())
//        return SideEffect::failure(RuntimeError(QString("%1 is not a string").arg(atom.toString())));
//    auto result = atom.convertString();
//    if (result.isFailure())
//        return SideEffect::failure(result.getError());
//    auto string = result.getResult();
//    if (string.size() < m_minLength)
//        return SideEffect::failure(RuntimeError(QString("%1 length is less than %2").arg(atom.toString()).arg(m_minLength)));
//    if (string.size() > m_maxLength)
//        return SideEffect::failure(RuntimeError(QString("%1 length is greater than %2").arg(atom.toString()).arg(m_maxLength)));
//
//    return SideEffect::success();
//}
//
//SchemaComparisonType
//SchemaString::compare(SchemaNode *other) const
//{
//    if (!other)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//
//    if (other->getType() != SchemaNodeType::SCHEMA_STRING) {
//        auto *otherString = static_cast<SchemaString *>(other);
//
//        if (m_minLength == otherString->getMinLength() &&
//            m_maxLength == otherString->getMaxLength() &&
//            m_pattern == otherString->getPattern())
//            return SchemaComparisonType::COMPARE_EQUAL;
//
//        if (m_minLength <= otherString->getMinLength() &&
//            m_maxLength >= otherString->getMaxLength() &&
//            m_pattern == otherString->getPattern())
//            return SchemaComparisonType::COMPARE_SUBSET;
//    }
//
//    QSet<Atom> values;
//    switch (other->getType()) {
//        case SchemaNodeType::SCHEMA_CONST:
//            values = {static_cast<SchemaConst *>(other)->getConst()};
//            break;
//        case SchemaNodeType::SCHEMA_ENUM:
//            values = static_cast<SchemaEnum *>(other)->getEnum();
//            break;
//        default:
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    for (const auto &value : values) {
//        if (!value.isString())
//            return SchemaComparisonType::COMPARE_DISJOINT;
//        auto string = value.getString();
//        if (string.size() < m_minLength || m_maxLength < string.size())
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    return SchemaComparisonType::COMPARE_SUBSET;
//}
//
//uint
//SchemaString::hash() const
//{
//    return qHash(m_minLength) + qHash(m_maxLength) + qHash(m_pattern);
//}
//
//SchemaNode *
//SchemaString::clone() const
//{
//    auto string = new SchemaString();
//    string->m_minLength = m_minLength;
//    string->m_maxLength = m_maxLength;
//    string->m_pattern = m_pattern;
//    return string;
//}
//
//SchemaSeq::SchemaSeq()
//{
//    m_minItems = 0;
//    m_maxItems = std::numeric_limits<qint64>::max();
//    m_items = nullptr;
//    m_uniqueItems = false;
//    m_contains = nullptr;
//}
//
//SchemaSeq::~SchemaSeq()
//{
//    delete m_items;
//    delete m_contains;
//}
//
//SchemaNodeType
//SchemaSeq::getType() const
//{
//    return SchemaNodeType::SCHEMA_SEQ;
//}
//
//SchemaValueType
//SchemaSeq::getValueType() const
//{
//    return SchemaValueType::VALUE_SEQ;
//}
//
//qint64
//SchemaSeq::getMinItems() const
//{
//    return m_minItems;
//}
//
//void
//SchemaSeq::setMinItems(qint64 minItems)
//{
//    m_minItems = minItems;
//}
//
//qint64
//SchemaSeq::getMaxItems() const
//{
//    return m_maxItems;
//}
//
//void
//SchemaSeq::setMaxItems(qint64 maxItems)
//{
//    m_maxItems = maxItems;
//}
//
//SchemaNode *
//SchemaSeq::getItems() const
//{
//    return m_items;
//}
//
//void
//SchemaSeq::setItems(SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    delete m_items;
//    m_items = node;
//    node->setParent(this);
//}
//
//bool
//SchemaSeq::getUniqueItems() const
//{
//    return m_uniqueItems;
//}
//
//void
//SchemaSeq::setUniqueItems(bool uniqueItems)
//{
//    m_uniqueItems = uniqueItems;
//}
//
//SchemaNode *
//SchemaSeq::getContains() const
//{
//    return m_contains;
//}
//
//void
//SchemaSeq::setContains(SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    delete m_contains;
//    m_contains = node;
//    node->setParent(this);
//}
//
//SideEffect
//SchemaSeq::validate(const Atom &atom)
//{
//    if (!atom.isSeq())
//        return SideEffect::failure(RuntimeError(QString("%1 is not a seq").arg(atom.toString())));
//    if (atom.seqSize() < m_minItems)
//        return SideEffect::failure(RuntimeError(QString("%1 item count is less than %2").arg(atom.toString()).arg(m_minItems)));
//    if (atom.seqSize() > m_maxItems)
//        return SideEffect::failure(RuntimeError(QString("%1 item count is greater than %2").arg(atom.toString()).arg(m_maxItems)));
//
//    bool contains = m_contains ? false : true;
//    for (auto iterator = atom.seqBegin(); iterator != atom.seqEnd(); iterator++) {
//        auto &item = *iterator;
//
//        if (m_items) {
//            auto result = m_items->validate(item);
//            if (result.isFailure())
//                return SideEffect::failure(RuntimeError(QString("%1 does not conform to schema: %2").arg(item.toString()).arg(result.getError().getMessage())));
//        }
//        if (m_contains && !contains && m_contains->validate(item).isSuccess())
//            contains = true;
//    }
//
//    if (!contains)
//        return SideEffect::failure(RuntimeError(QString("%1 does not contain item conforming to schema").arg(atom.toString())));
//
//    return SideEffect::success();
//
//}
//
//SchemaComparisonType
//SchemaSeq::compare(SchemaNode *other) const
//{
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaSeq::hash() const
//{
//    auto result = qHash(m_minItems) + qHash(m_maxItems) + qHash(m_uniqueItems);
//    if (m_items)
//        result += m_items->hash();
//    if (m_contains)
//        result += m_contains->hash();
//    return result;
//}
//
//SchemaNode *
//SchemaSeq::clone() const
//{
//    SchemaSeq *array = new SchemaSeq;
//    array->m_minItems = m_minItems;
//    array->m_maxItems = m_maxItems;
//    array->m_uniqueItems = m_uniqueItems;
//    if (m_items)
//        array->m_items = m_items->clone();
//    if (m_contains)
//        array->m_contains = m_contains->clone();
//    return array;
//}
//
//SchemaMap::SchemaMap() : SchemaNode()
//{
//    m_minProperties = 0;
//    m_maxProperties = std::numeric_limits<qint64>::max();
//}
//
//SchemaMap::~SchemaMap()
//{
//    for (auto iterator = m_properties.begin(); iterator != m_properties.end(); iterator++) {
//        delete iterator.value();
//    }
//    m_properties.clear();
//}
//
//SchemaNodeType
//SchemaMap::getType() const
//{
//    return SchemaNodeType::SCHEMA_MAP;
//}
//
//SchemaValueType
//SchemaMap::getValueType() const
//{
//    return SchemaValueType::VALUE_MAP;
//}
//
//qint64
//SchemaMap::getMinProperties() const
//{
//    return m_minProperties;
//}
//
//void
//SchemaMap::setMinProperties(qint64 minProperties)
//{
//    m_minProperties = minProperties;
//}
//
//qint64
//SchemaMap::getMaxProperties() const
//{
//    return m_maxProperties;
//}
//
//void
//SchemaMap::setMaxProperties(qint64 maxProperties)
//{
//    m_maxProperties = maxProperties;
//}
//
//QStringList
//SchemaMap::getRequired() const
//{
//    return QList<QString>(m_required.cbegin(), m_required.cend());
//}
//
//void
//SchemaMap::setRequired(const QStringList &required)
//{
//    m_required = QSet<QString>(required.cbegin(), required.cend());
//}
//
//SchemaNode *
//SchemaMap::getProperty(const QString &name) const
//{
//    if (!m_properties.contains(name))
//        return nullptr;
//    return m_properties[name];
//}
//
//void
//SchemaMap::setProperty(const QString &name, SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    removeProperty(name);
//    m_properties[name] = node;
//    node->setParent(this);
//}
//
//void
//SchemaMap::removeProperty(const QString &name)
//{
//    if (m_properties.contains(name))
//        delete m_properties.take(name);
//}
//
//int
//SchemaMap::numProperties() const
//{
//    return m_properties.size();
//}
//
//SideEffect
//SchemaMap::validate(const Atom &atom)
//{
//    if (!atom.isMap())
//        return SideEffect::failure(RuntimeError(QString("%1 is not a map").arg(atom.toString())));
//    if (atom.mapSize() < m_minProperties)
//        return SideEffect::failure(RuntimeError(QString("%1 property count is less than %2").arg(atom.toString()).arg(m_minProperties)));
//    if (atom.mapSize() > m_maxProperties)
//        return SideEffect::failure(RuntimeError(QString("%1 property count is greater than %2").arg(atom.toString()).arg(m_maxProperties)));
//    if (!m_required.isEmpty()) {
//        auto missingKeys = QSet<QString>(atom.mapKeys().cbegin(), atom.mapKeys().cend());
//        missingKeys.subtract(m_required);
//        if (!missingKeys.isEmpty()) {
//            return SideEffect::failure(
//                RuntimeError(QString("%1 is missing required keys %2")
//                .arg(atom.toString())
//                .arg(QStringList(missingKeys.cbegin(), missingKeys.cend()).join(", "))));
//        }
//    }
//    for (auto iterator = m_properties.cbegin(); iterator != m_properties.cend(); iterator++) {
//        auto name = iterator.key();
//        auto schema = iterator.value();
//        if (!atom.mapContains(name))
//            return SideEffect::failure(RuntimeError(QString("%1 is missing required property %2").arg(atom.toString()).arg(name)));
//        auto result = schema->validate(atom.mapAt(name));
//        if (result.isFailure())
//            return result;
//    }
//    return SideEffect::success();
//}
//
//SchemaComparisonType
//SchemaMap::compare(SchemaNode *other) const
//{
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaMap::hash() const
//{
//    auto result = qHash(m_minProperties) + qHash(m_maxProperties) + qHash(m_required);
//    auto propertyKeys = m_properties.keys();
//    std::sort(propertyKeys.begin(), propertyKeys.end());
//    for (const auto &key : propertyKeys) {
//        result += qHash(key);
//        result += m_properties[key]->hash();
//    }
//    return result;
//}
//
//SchemaNode *
//SchemaMap::clone() const
//{
//    auto object = new SchemaMap();
//    object->m_minProperties = m_minProperties;
//    object->m_maxProperties = m_maxProperties;
//    object->m_required = m_required;
//    for (auto iterator = m_properties.cbegin(); iterator != m_properties.cend(); iterator++) {
//        object->m_properties[iterator.key()] = iterator.value()->clone();
//    }
//    return object;
//}
//
//SchemaClass::SchemaClass()
//{
//}
//
//SchemaNodeType
//SchemaClass::getType() const
//{
//    return SchemaNodeType::SCHEMA_CLASS;
//}
//
//SchemaValueType
//SchemaClass::getValueType() const
//{
//    return SchemaValueType::VALUE_CLASS;
//}
//
//QVector<uint16_t>
//SchemaClass::getAncestry() const
//{
//    return m_ancestry;
//}
//
//void
//SchemaClass::setAncestry(const QVector<uint16_t> &ancestry)
//{
//    m_ancestry = ancestry;
//}
//
//SideEffect
//SchemaClass::validate(const Atom &atom)
//{
//    return SideEffect::failure(RuntimeError(QString("%1 is not an object").arg(atom.toString())));
//}
//
//SchemaComparisonType
//SchemaClass::compare(SchemaNode *other) const
//{
//    if (!other)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//
//    if (other->getType() != SchemaNodeType::SCHEMA_CLASS)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//
//    auto otherAncestry = static_cast<SchemaClass *>(other)->getAncestry();
//    if (otherAncestry.size() < m_ancestry.size())
//        return SchemaComparisonType::COMPARE_DISJOINT;
//
//    for (int i = 0; i < m_ancestry.size(); i++) {
//        if (m_ancestry[i] != otherAncestry[i])
//            return SchemaComparisonType::COMPARE_DISJOINT;
//    }
//
//    if (otherAncestry.size() == m_ancestry.size())
//        return SchemaComparisonType::COMPARE_EQUAL;
//    return SchemaComparisonType::COMPARE_SUBSET;
//}
//
//uint
//SchemaClass::hash() const
//{
//    return qHash(m_ancestry);
//}
//
//SchemaNode *
//SchemaClass::clone() const
//{
//    auto object = new SchemaClass();
//    object->m_ancestry = m_ancestry;
//    return object;
//}
//
//SchemaAnyOf::SchemaAnyOf() : SchemaNode()
//{
//}
//
//SchemaNodeType
//SchemaAnyOf::getType() const
//{
//    return SchemaNodeType::SCHEMA_ANY_OF;
//}
//
//SchemaValueType
//SchemaAnyOf::getValueType() const
//{
//    if (m_anyOf.isEmpty())
//        return SchemaValueType::VALUE_ANY;
//    SchemaValueType valueType = m_anyOf.first()->getValueType();
//    if (m_anyOf.size() == 1)
//        return valueType;
//    for (int i = 1; i < m_anyOf.size(); i++) {
//        SchemaValueType otherType = m_anyOf[i]->getValueType();
//        if (otherType != valueType)
//            return SchemaValueType::VALUE_ANY_OF;
//    }
//    return valueType;
//}
//
//SchemaNode *
//SchemaAnyOf::getNode(int index) const
//{
//    Q_ASSERT(0 <= index && index < m_anyOf.size());
//    auto node = m_anyOf[index];
//    Q_ASSERT(node != nullptr);
//    return node;
//}
//
//void
//SchemaAnyOf::appendNode(SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    m_anyOf.append(node);
//    node->setParent(this);
//}
//
//void
//SchemaAnyOf::insertNode(int index, SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    m_anyOf.insert(index, node);
//    node->setParent(this);
//}
//
//void
//SchemaAnyOf::removeNode(int index)
//{
//    Q_ASSERT(0 <= index && index < m_anyOf.size());
//    auto node = m_anyOf.takeAt(index);
//    Q_ASSERT(node != nullptr);
//    delete node;
//}
//
//int
//SchemaAnyOf::numNodes() const
//{
//    return m_anyOf.size();
//}
//
//SideEffect
//SchemaAnyOf::validate(const Atom &atom)
//{
//    QStringList errors;
//
//    for (auto &schema : m_anyOf) {
//        auto result = schema->validate(atom);
//        if (result.isSuccess())
//            return result;
//        errors.append(result.getError().getMessage());
//    }
//    return SideEffect::failure(RuntimeError(QString("%1 does not conform to any schema: [ %2 ]").arg(atom.toString()).arg(errors.join(", "))));
//}
//
//SchemaComparisonType
//SchemaAnyOf::compare(SchemaNode *other) const
//{
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaAnyOf::hash() const
//{
//    uint result = 0;
//    for (const auto &node : m_anyOf) {
//        result += node->hash();
//    }
//    return result;
//}
//
//SchemaNode *
//SchemaAnyOf::clone() const
//{
//    auto anyof = new SchemaAnyOf();
//    for (auto &schema : m_anyOf) {
//        anyof->m_anyOf.append(schema->clone());
//    }
//    return anyof;
//}
//
//SchemaAllOf::SchemaAllOf() : SchemaNode()
//{
//}
//
//SchemaNodeType
//SchemaAllOf::getType() const
//{
//    return SchemaNodeType::SCHEMA_ALL_OF;
//}
//
//SchemaValueType
//SchemaAllOf::getValueType() const
//{
//    if (m_allOf.isEmpty())
//        return SchemaValueType::VALUE_ANY;
//    SchemaValueType valueType = m_allOf.first()->getValueType();
//    if (m_allOf.size() == 1)
//        return valueType;
//    for (int i = 1; i < m_allOf.size(); i++) {
//        SchemaValueType otherType = m_allOf[i]->getValueType();
//        if (otherType != valueType)
//            return SchemaValueType::VALUE_ALL_OF;
//    }
//    return valueType;
//}
//
//SchemaNode *
//SchemaAllOf::getNode(int index) const
//{
//    Q_ASSERT(0 <= index && index < m_allOf.size());
//    auto node = m_allOf[index];
//    Q_ASSERT(node != nullptr);
//    return node;
//}
//
//void
//SchemaAllOf::appendNode(SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    m_allOf.append(node);
//    node->setParent(this);
//}
//
//void
//SchemaAllOf::insertNode(int index, SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    m_allOf.insert(index, node);
//    node->setParent(this);
//}
//
//void
//SchemaAllOf::removeNode(int index)
//{
//    Q_ASSERT(0 <= index && index < m_allOf.size());
//    auto node = m_allOf.takeAt(index);
//    Q_ASSERT(node != nullptr);
//    delete node;
//}
//
//int
//SchemaAllOf::numNodes() const
//{
//    return m_allOf.size();
//}
//
//SideEffect
//SchemaAllOf::validate(const Atom &atom)
//{
//    QStringList errors;
//
//    for (auto &schema : m_allOf) {
//        auto result = schema->validate(atom);
//        if (result.isFailure())
//            errors.append(result.getError().getMessage());
//    }
//    if (errors.isEmpty())
//        return SideEffect::success();
//    return SideEffect::failure(RuntimeError(QString("%1 does not conform to all schemas: [ %2 ]").arg(atom.toString()).arg(errors.join(", "))));
//}
//
//SchemaComparisonType
//SchemaAllOf::compare(SchemaNode *other) const
//{
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaAllOf::hash() const
//{
//    uint result = 0;
//    for (const auto &node : m_allOf) {
//        result += node->hash();
//    }
//    return result;
//}
//
//SchemaNode *
//SchemaAllOf::clone() const
//{
//    auto allof = new SchemaAllOf();
//    for (auto &schema : m_allOf) {
//        allof->m_allOf.append(schema->clone());
//    }
//    return allof;
//}
//
//SchemaNot::SchemaNot()
//{
//    m_not = nullptr;
//}
//
//SchemaNot::~SchemaNot()
//{
//    delete m_not;
//}
//
//SchemaNodeType
//SchemaNot::getType() const
//{
//    return SchemaNodeType::SCHEMA_NOT;
//}
//
//SchemaValueType
//SchemaNot::getValueType() const
//{
//    if (m_not == nullptr)
//        return SchemaValueType::VALUE_ANY;
//    return SchemaValueType::VALUE_NOT;
//}
//
//SchemaNode *
//SchemaNot::getNode() const
//{
//    return m_not;
//}
//
//void
//SchemaNot::setNode(SchemaNode *node)
//{
//    Q_ASSERT(node != nullptr);
//    delete m_not;
//    m_not = node;
//    node->setParent(this);
//}
//
//SideEffect
//SchemaNot::validate(const Atom &atom)
//{
//    if (m_not) {
//        auto result = m_not->validate(atom);
//        if (!result.isFailure())
//            return SideEffect::failure(RuntimeError(QString("%1 does conform to schema").arg(atom.toString())));
//    }
//    return SideEffect::success();
//}
//
//SchemaComparisonType
//SchemaNot::compare(SchemaNode *other) const
//{
//    return SchemaComparisonType::COMPARE_DISJOINT;
//}
//
//uint
//SchemaNot::hash() const
//{
//    return m_not != nullptr? m_not->hash() : 0;
//}
//
//SchemaNode *
//SchemaNot::clone() const
//{
//    auto *not_ = new SchemaNot;
//    if (m_not)
//        not_->m_not = m_not->clone();
//    return not_;
//}
//
//QDebug
//operator<<(QDebug dbg, const SchemaNodeType &t)
//{
//    switch (t) {
//        case SchemaNodeType::SCHEMA_EMPTY:
//            dbg.noquote() << "EMPTY";
//            break;
//        case SchemaNodeType::SCHEMA_ANY:
//            dbg.noquote() << "ANY";
//            break;
//        case SchemaNodeType::SCHEMA_CONST:
//            dbg.noquote() << "CONST";
//            break;
//        case SchemaNodeType::SCHEMA_ENUM:
//            dbg.noquote() << "ENUM";
//            break;
//        case SchemaNodeType::SCHEMA_INTEGER:
//            dbg.noquote() << "INTEGER";
//            break;
//        case SchemaNodeType::SCHEMA_FLOAT:
//            dbg.noquote() << "FLOAT";
//            break;
//        case SchemaNodeType::SCHEMA_STRING:
//            dbg.noquote() << "STRING";
//            break;
//        case SchemaNodeType::SCHEMA_SEQ:
//            dbg.noquote() << "SEQ";
//            break;
//        case SchemaNodeType::SCHEMA_MAP:
//            dbg.noquote() << "MAP";
//            break;
//        case SchemaNodeType::SCHEMA_CLASS:
//            dbg.noquote() << "CLASS";
//            break;
//        case SchemaNodeType::SCHEMA_ANY_OF:
//            dbg.noquote() << "ANY_OF";
//            break;
//        case SchemaNodeType::SCHEMA_ALL_OF:
//            dbg.noquote() << "ALL_OF";
//            break;
//        case SchemaNodeType::SCHEMA_NOT:
//            dbg.noquote() << "NOT";
//            break;
//    }
//    return dbg;
//}
