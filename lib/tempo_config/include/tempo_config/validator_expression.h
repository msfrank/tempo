//#ifndef SCHEMA_EXPRESSION_H
//#define SCHEMA_EXPRESSION_H
//
//#include <QList>
//#include <QSet>
//#include <QMap>
//#include <QString>
//
//#include "atom.h"
//#include "option_template.h"
//#include "schema_node.h"
//
//class SchemaExpression {
//
//public:
//    SchemaExpression();
//    explicit SchemaExpression(const SchemaNode *node);
//    SchemaExpression(const SchemaExpression &other);
//    ~SchemaExpression();
//
//    SchemaExpression& operator=(const SchemaExpression &other);
//
//    SchemaNodeType getType() const;
//    SchemaValueType getValueType() const;
//    SideEffect validate(const Atom &value);
//    SchemaComparisonType compare(const SchemaExpression &other) const;
//
//    bool operator==(const SchemaExpression &other) const;
//    bool operator!=(const SchemaExpression &other) const;
//    uint hash() const;
//
//    QString toString() const;
//    SchemaNode *toNode() const;
//
//    static SchemaExpression forAny();
//    static SchemaExpression forConst(const Atom &value);
//    static SchemaExpression forEnum(const QSet<Atom> &value);
//    static SchemaExpression forNil();
//    static SchemaExpression forBoolean();
//    static SchemaExpression forInteger(qint64 minimum = std::numeric_limits<qint64>::min(),
//                                       qint64 maximum = std::numeric_limits<qint64>::max(),
//                                       bool minExclusive = false,
//                                       bool maxExclusive = false,
//                                       qint64 multipleOf = 1);
//    static SchemaExpression forFloat(double minimum = std::numeric_limits<double>::min(),
//                                     double maximum = std::numeric_limits<double>::max(),
//                                     bool minExclusive = false,
//                                     bool maxExclusive = false);
//    static SchemaExpression forString(qint64 minLength = std::numeric_limits<qint64>::min(),
//                                      qint64 maxLength = std::numeric_limits<qint64>::max(),
//                                      const QString &pattern = QString());
//    static SchemaExpression forSeq();
//    static SchemaExpression forMap();
//    static SchemaExpression forClass(const QVector<tu_uint16> &ancestry);
//    static SchemaExpression forAnyOf(const QList<SchemaExpression> &value);
//    static SchemaExpression forAllOf(const QList<SchemaExpression> &value);
//    static SchemaExpression forNot(const SchemaExpression &value);
//
//private:
//    SchemaNode *m_node;
//
//    explicit SchemaExpression(SchemaNode *node, bool copy);
//};
//
//QDebug operator<<(QDebug debug, const SchemaExpression &expression);
//uint qHash(const SchemaExpression &expression);
//
//#endif // SCHEMA_EXPRESSION_H
