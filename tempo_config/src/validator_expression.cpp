//
//#include "schema_expression.h"
//
//SchemaExpression::SchemaExpression()
//{
//    m_node = nullptr;
//}
//
//SchemaExpression::SchemaExpression(const SchemaNode *node)
//{
//    Q_ASSERT(node);
//    m_node = node->clone();
//}
//
//SchemaExpression::SchemaExpression(SchemaNode *node, bool copy)
//{
//    Q_ASSERT(node);
//    if (copy)
//        m_node = node->clone();
//    else
//        m_node = node;
//}
//
//SchemaExpression::SchemaExpression(const SchemaExpression &other)
//{
//    if (other.m_node)
//        m_node = other.m_node->clone();
//    else
//        m_node = nullptr;
//}
//
//SchemaExpression::~SchemaExpression()
//{
//    delete m_node;
//}
//
//SchemaExpression& SchemaExpression::operator=(const SchemaExpression &other)
//{
//    if (this != &other) {
//        delete m_node;
//        if (other.m_node)
//            m_node = other.m_node->clone();
//        else
//            m_node = nullptr;
//    }
//    return *this;
//}
//
//SchemaNodeType
//SchemaExpression::getType() const
//{
//    if (m_node)
//        return m_node->getType();
//    return SchemaNodeType::SCHEMA_EMPTY;
//}
//
//SchemaValueType
//SchemaExpression::getValueType() const
//{
//    if (m_node)
//        return m_node->getValueType();
//    return SchemaValueType::VALUE_EMPTY;
//}
//
//bool
//SchemaExpression::operator==(const SchemaExpression &other) const
//{
//    if (!m_node && !other.m_node)
//        return true;
//    if (!m_node || !other.m_node)
//        return false;
//    return m_node->compare(other.m_node) == SchemaComparisonType::COMPARE_EQUAL;
//}
//
//bool
//SchemaExpression::operator!=(const SchemaExpression &other) const
//{
//    return !(this->operator==(other));
//}
//
//uint
//SchemaExpression::hash() const
//{
//    return m_node? m_node->hash() : 0;
//}
//
//SideEffect
//SchemaExpression::validate(const Atom &value)
//{
//    if (m_node)
//        return m_node->validate(value);
//    return SideEffect::failure(RuntimeError("nothing matches the empty type"));
//}
//
//SchemaComparisonType
//SchemaExpression::compare(const SchemaExpression &other) const
//{
//    if (!m_node || !other.m_node)
//        return SchemaComparisonType::COMPARE_DISJOINT;
//    return m_node->compare(other.m_node);
//}
//
//QString
//SchemaExpression::toString() const
//{
//    return QString();
//}
//
//SchemaNode *
//SchemaExpression::toNode() const
//{
//    if (m_node)
//        return m_node->clone();
//    return nullptr;
//}
//
//SchemaExpression
//SchemaExpression::forAny()
//{
//    auto node = new SchemaAny();
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forConst(const Atom &value)
//{
//    auto node = new SchemaConst();
//    node->setConst(value);
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forEnum(const QSet<Atom> &value)
//{
//    auto node = new SchemaEnum();
//    node->setEnum(value);
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forNil()
//{
//    return SchemaExpression::forEnum({Atom()});
//}
//
//SchemaExpression
//SchemaExpression::forBoolean()
//{
//    return SchemaExpression::forEnum({Atom(true), Atom(false)});
//}
//
//SchemaExpression
//SchemaExpression::forInteger(qint64 minimum, qint64 maximum, bool minExclusive, bool maxExclusive, qint64 multipleOf)
//{
//    auto node = new SchemaInteger();
//    node->setMinimum(minimum);
//    node->setMaximum(maximum);
//    node->setMinExclusive(minExclusive);
//    node->setMaxExclusive(maxExclusive);
//    node->setMultipleOf(multipleOf);
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forFloat(double minimum, double maximum, bool minExclusive, bool maxExclusive)
//{
//    auto node = new SchemaFloat();
//    node->setMinimum(minimum);
//    node->setMaximum(maximum);
//    node->setMinExclusive(minExclusive);
//    node->setMaxExclusive(maxExclusive);
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forString(qint64 minLength, qint64 maxLength, const QString &pattern)
//{
//    auto node = new SchemaString();
//    node->setMinLength(minLength);
//    node->setMaxLength(maxLength);
//    node->setPattern(pattern);
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forSeq()
//{
//    auto node = new SchemaSeq();
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forMap()
//{
//    auto node = new SchemaMap();
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forClass(const QVector<uint16_t> &ancestry)
//{
//    auto node = new SchemaClass();
//    node->setAncestry(ancestry);
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forAnyOf(const QList<SchemaExpression> &anyOf)
//{
//    auto node = new SchemaAnyOf();
//    for (const auto &value : anyOf) {
//        node->appendNode(value.toNode());
//    }
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forAllOf(const QList<SchemaExpression> &allOf)
//{
//    auto node = new SchemaAllOf();
//    for (const auto &value : allOf) {
//        node->appendNode(value.toNode());
//    }
//    return SchemaExpression(node, false);
//}
//
//SchemaExpression
//SchemaExpression::forNot(const SchemaExpression &value)
//{
//    auto node = new SchemaNot();
//    node->setNode(value.toNode());
//    return SchemaExpression(node, false);
//}
//
//QDebug operator<<(QDebug debug, const SchemaExpression &expression)
//{
//    debug << "SchemaExpression()";
//    return debug;
//}
//
//uint qHash(const SchemaExpression &expression)
//{
//    return expression.hash();
//}
//
