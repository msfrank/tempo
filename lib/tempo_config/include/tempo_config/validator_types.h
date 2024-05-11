//#ifndef SCHEMA_NODE_H
//#define SCHEMA_NODE_H
//
//#include <QString>
//#include <QHash>
//#include <QSet>
//#include <QList>
//
//#include "atom.h"
//#include "side_effect.h"
//
//enum class SchemaNodeType {
//    SCHEMA_EMPTY,
//    SCHEMA_ANY,
//    SCHEMA_CONST,
//    SCHEMA_ENUM,
//    SCHEMA_INTEGER,
//    SCHEMA_FLOAT,
//    SCHEMA_STRING,
//    SCHEMA_SEQ,
//    SCHEMA_MAP,
//    SCHEMA_CLASS,
//    SCHEMA_ANY_OF,
//    SCHEMA_ALL_OF,
//    SCHEMA_NOT
//};
//
//enum class SchemaValueType {
//    VALUE_EMPTY,
//    VALUE_ANY,
//    VALUE_NULL,
//    VALUE_BOOL,
//    VALUE_INTEGER,
//    VALUE_FLOAT,
//    VALUE_STRING,
//    VALUE_SEQ,
//    VALUE_MAP,
//    VALUE_CLASS,
//    VALUE_ANY_OF,
//    VALUE_ALL_OF,
//    VALUE_NOT
//};
//
//QDebug operator<<(QDebug debug, const SchemaNodeType &t);
//
//enum class SchemaComparisonType {
//    COMPARE_DISJOINT,
//    COMPARE_SUBSET,
//    COMPARE_EQUAL,
//};
//
//class SchemaNode {
//
//public:
//    SchemaNode();
//    virtual ~SchemaNode() = default;
//
//    virtual SchemaNodeType getType() const = 0;
//    virtual SchemaValueType getValueType() const = 0;
//    SchemaNode *getParent() const;
//    void setParent(SchemaNode *parent);
//
//    virtual SideEffect validate(const Atom &atom) = 0;
//    virtual SchemaComparisonType compare(SchemaNode *other) const = 0;
//    virtual uint hash() const = 0;
//
//    virtual SchemaNode *clone() const = 0;
//
//private:
//    SchemaNode *m_parent;
//};
//
//class SchemaAny : public SchemaNode {
//
//public:
//    SchemaAny() = default;
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//    SchemaNode *clone() const override;
//};
//
//class SchemaConst : public SchemaNode {
//
//public:
//    SchemaConst();
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    Atom getConst() const;
//    void setConst(const Atom &const_);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    Atom m_const;
//};
//
//class SchemaEnum : public SchemaNode {
//
//public:
//    SchemaEnum();
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    QSet<Atom> getEnum() const;
//    void setEnum(const QSet<Atom> &enum_);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    QSet<Atom> m_enum;
//};
//
//class SchemaInteger : public SchemaNode {
//
//public:
//    SchemaInteger();
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    qint64 getMultipleOf() const;
//    void setMultipleOf(qint64 multipleOf);
//    qint64 getMinimum() const;
//    void setMinimum(qint64 minimum);
//    bool getMinExclusive() const;
//    void setMinExclusive(bool exclusive);
//    qint64 getMaximum() const;
//    void setMaximum(qint64 maximum);
//    bool getMaxExclusive() const;
//    void setMaxExclusive(bool exclusive);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    qint64 m_multipleOf;
//    qint64 m_minimum;
//    bool m_minExclusive;
//    qint64 m_maximum;
//    bool m_maxExclusive;
//};
//
//class SchemaFloat : public SchemaNode {
//
//public:
//    SchemaFloat();
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    double getMinimum() const;
//    void setMinimum(double minimum);
//    bool getMinExclusive() const;
//    void setMinExclusive(bool exclusive);
//    double getMaximum() const;
//    void setMaximum(double maximum);
//    bool getMaxExclusive() const;
//    void setMaxExclusive(bool exclusive);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    double m_minimum;
//    bool m_minExclusive;
//    double m_maximum;
//    bool m_maxExclusive;
//};
//
//class SchemaString : public SchemaNode {
//
//public:
//    SchemaString();
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    qint64 getMinLength() const;
//    void setMinLength(qint64 minLength);
//    qint64 getMaxLength() const;
//    void setMaxLength(qint64 maxLength);
//    QString getPattern() const;
//    void setPattern(const QString &pattern);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    qint64 m_minLength;
//    qint64 m_maxLength;
//    QString m_pattern;
//};
//
//class SchemaSeq : public SchemaNode {
//
//public:
//    SchemaSeq();
//    ~SchemaSeq() override;
//
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    qint64 getMinItems() const;
//    void setMinItems(qint64 minItems);
//    qint64 getMaxItems() const;
//    void setMaxItems(qint64 maxItems);
//    SchemaNode *getItems() const;
//    void setItems(SchemaNode *node);
//    bool getUniqueItems() const;
//    void setUniqueItems(bool uniqueItems);
//    SchemaNode *getContains() const;
//    void setContains(SchemaNode *node);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    qint64 m_minItems;
//    qint64 m_maxItems;
//    SchemaNode *m_items;
//    bool m_uniqueItems;
//    SchemaNode *m_contains;
//};
//
//class SchemaMap : public SchemaNode {
//
//public:
//    SchemaMap();
//    ~SchemaMap() override;
//
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    qint64 getMinProperties() const;
//    void setMinProperties(qint64 minProperties);
//    qint64 getMaxProperties() const;
//    void setMaxProperties(qint64 maxProperties);
//    QStringList getRequired() const;
//    void setRequired(const QStringList &required);
//
//    SchemaNode *getProperty(const QString &name) const;
//    void setProperty(const QString &name, SchemaNode *node);
//    void removeProperty(const QString &name);
//    int numProperties() const;
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    qint64 m_minProperties;
//    qint64 m_maxProperties;
//    QSet<QString> m_required;
//    QHash<QString,SchemaNode *> m_properties;
//};
//
//class SchemaClass : public SchemaNode {
//
//public:
//    SchemaClass();
//
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    QVector<tu_uint16> getAncestry() const;
//    void setAncestry(const QVector<tu_uint16> &ancestry);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    QVector<tu_uint16> m_ancestry;
//};
//
//class SchemaAnyOf : public SchemaNode {
//
//public:
//    SchemaAnyOf();
//
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    SchemaNode *getNode(int index) const;
//    void appendNode(SchemaNode *node);
//    void insertNode(int index, SchemaNode *node);
//    void removeNode(int index);
//    int numNodes() const;
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    QList<SchemaNode *> m_anyOf;
//};
//
//class SchemaAllOf : public SchemaNode {
//
//public:
//    SchemaAllOf();
//
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    SchemaNode *getNode(int index) const;
//    void appendNode(SchemaNode *node);
//    void insertNode(int index, SchemaNode *node);
//    void removeNode(int index);
//    int numNodes() const;
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    QList<SchemaNode *> m_allOf;
//};
//
//class SchemaNot : public SchemaNode {
//
//public:
//    SchemaNot();
//    ~SchemaNot() override;
//
//    SchemaNodeType getType() const override;
//    SchemaValueType getValueType() const override;
//
//    SchemaNode *getNode() const;
//    void setNode(SchemaNode *node);
//
//    SideEffect validate(const Atom &atom) override;
//    SchemaComparisonType compare(SchemaNode *other) const override;
//    uint hash() const override;
//
//    SchemaNode *clone() const override;
//
//private:
//    SchemaNode *m_not;
//};
//
//#endif // SCHEMA_NODE_H
