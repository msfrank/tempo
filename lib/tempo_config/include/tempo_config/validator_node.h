#ifndef TEMPO_CONFIG_VALIDATOR_TYPES_H
#define TEMPO_CONFIG_VALIDATOR_TYPES_H

#include <tempo_utils/status.h>

#include "config_types.h"

namespace tempo_config {

    enum class ValidatorNodeType {
        SCHEMA_EMPTY,
        SCHEMA_ANY,
        SCHEMA_CONST,
        SCHEMA_ENUM,
        SCHEMA_INTEGER,
        SCHEMA_FLOAT,
        SCHEMA_STRING,
        SCHEMA_SEQ,
        SCHEMA_MAP,
        SCHEMA_CLASS,
        SCHEMA_ANY_OF,
        SCHEMA_ALL_OF,
        SCHEMA_NOT
    };

    enum class ValidatorValueType {
        VALUE_EMPTY,
        VALUE_ANY,
        VALUE_NULL,
        VALUE_BOOL,
        VALUE_INTEGER,
        VALUE_FLOAT,
        VALUE_STRING,
        VALUE_SEQ,
        VALUE_MAP,
        VALUE_CLASS,
        VALUE_ANY_OF,
        VALUE_ALL_OF,
        VALUE_NOT
    };

    enum class ValidatorComparisonType {
        COMPARE_DISJOINT,
        COMPARE_SUBSET,
        COMPARE_EQUAL,
    };

    class ValidatorNode {

    public:
        virtual ~ValidatorNode() = default;

        virtual ValidatorNodeType getType() const = 0;
        virtual ValidatorValueType getValueType() const = 0;
        virtual tempo_utils::Status validate(const ConfigNode &node) = 0;
    };

    class ValidateAny : public ValidatorNode {
    public:
        ValidateAny() = default;
        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;
    };

    // class ValidatorConst : public ValidatorNode {
    //
    // public:
    //     ValidatorConst();
    //     ValidatorNodeType getType() const override;
    //     ValidatorValueType getValueType() const override;
    //
    //     Atom getConst() const;
    //     void setConst(const Atom &const_);
    //
    //     SideEffect validate(const Atom &atom) override;
    //     ValidatorComparisonType compare(ValidatorNode *other) const override;
    //     uint hash() const override;
    //
    //     ValidatorNode *clone() const override;
    //
    // private:
    //     Atom m_const;
    // };

    // class ValidatorEnum : public ValidatorNode {
    //
    // public:
    //     ValidatorEnum();
    //     ValidatorNodeType getType() const override;
    //     ValidatorValueType getValueType() const override;
    //
    //     QSet<Atom> getEnum() const;
    //     void setEnum(const QSet<Atom> &enum_);
    //
    //     SideEffect validate(const Atom &atom) override;
    //     ValidatorComparisonType compare(ValidatorNode *other) const override;
    //     uint hash() const override;
    //
    //     ValidatorNode *clone() const override;
    //
    // private:
    //     QSet<Atom> m_enum;
    // };

    class ValidateInteger : public ValidatorNode {

    public:
        explicit ValidateInteger(
            tu_int64 minimum = std::numeric_limits<tu_int64>::min(),
            tu_int64 maximum = std::numeric_limits<tu_int64>::max(),
            tu_int64 multipleOf = 1,
            bool minExclusive = false,
            bool maxExclusive = false);

        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;

        tu_int64 getMinimum() const;
        tu_int64 getMaximum() const;
        tu_int64 getMultipleOf() const;
        bool getMinExclusive() const;
        bool getMaxExclusive() const;

        tempo_utils::Status validate(const ConfigNode &node) override;

    private:
        tu_int64 m_minimum;
        tu_int64 m_maximum;
        tu_int64 m_multipleOf;
        bool m_minExclusive;
        bool m_maxExclusive;
    };

    class ValidateFloat : public ValidatorNode {

    public:
        explicit ValidateFloat(
            double minimum = std::numeric_limits<double>::lowest(),
            double maximum = std::numeric_limits<double>::max(),
            bool minExclusive = false,
            bool maxExclusive = false);

        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        double getMinimum() const;
        double getMaximum() const;
        bool getMinExclusive() const;
        bool getMaxExclusive() const;

    private:
        double m_minimum;
        double m_maximum;
        bool m_minExclusive;
        bool m_maxExclusive;
    };

    class ValidateString : public ValidatorNode
    {
    public:
        explicit ValidateString(
            tu_int32 minLength = 0,
            tu_int32 maxLength = std::numeric_limits<tu_int32>::max(),
            std::string_view pattern = {});

        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        tu_int32 getMinLength() const;
        tu_int32 getMaxLength() const;
        std::string getPattern() const;


    private:
        tu_int32 m_minLength;
        tu_int32 m_maxLength;
        std::string m_pattern;
    };

    // class ValidatorSeq : public ValidatorNode {
    //
    // public:
    //     ValidatorSeq();
    //     ~ValidatorSeq() override;
    //
    //     ValidatorNodeType getType() const override;
    //     ValidatorValueType getValueType() const override;
    //
    //     qint64 getMinItems() const;
    //     void setMinItems(qint64 minItems);
    //     qint64 getMaxItems() const;
    //     void setMaxItems(qint64 maxItems);
    //     ValidatorNode *getItems() const;
    //     void setItems(ValidatorNode *node);
    //     bool getUniqueItems() const;
    //     void setUniqueItems(bool uniqueItems);
    //     ValidatorNode *getContains() const;
    //     void setContains(ValidatorNode *node);
    //
    //     SideEffect validate(const Atom &atom) override;
    //     ValidatorComparisonType compare(ValidatorNode *other) const override;
    //     uint hash() const override;
    //
    //     ValidatorNode *clone() const override;
    //
    // private:
    //     qint64 m_minItems;
    //     qint64 m_maxItems;
    //     ValidatorNode *m_items;
    //     bool m_uniqueItems;
    //     ValidatorNode *m_contains;
    // };
    //
    // class ValidatorMap : public ValidatorNode {
    //
    // public:
    //     ValidatorMap();
    //     ~ValidatorMap() override;
    //
    //     ValidatorNodeType getType() const override;
    //     ValidatorValueType getValueType() const override;
    //
    //     qint64 getMinProperties() const;
    //     void setMinProperties(qint64 minProperties);
    //     qint64 getMaxProperties() const;
    //     void setMaxProperties(qint64 maxProperties);
    //     QStringList getRequired() const;
    //     void setRequired(const QStringList &required);
    //
    //     ValidatorNode *getProperty(const QString &name) const;
    //     void setProperty(const QString &name, ValidatorNode *node);
    //     void removeProperty(const QString &name);
    //     int numProperties() const;
    //
    //     SideEffect validate(const Atom &atom) override;
    //     ValidatorComparisonType compare(ValidatorNode *other) const override;
    //     uint hash() const override;
    //
    //     ValidatorNode *clone() const override;
    //
    // private:
    //     qint64 m_minProperties;
    //     qint64 m_maxProperties;
    //     QSet<QString> m_required;
    //     QHash<QString,ValidatorNode *> m_properties;
    // };

    // class ValidatorClass : public ValidatorNode {
    //
    // public:
    //     ValidatorClass();
    //
    //     ValidatorNodeType getType() const override;
    //     ValidatorValueType getValueType() const override;
    //
    //     QVector<tu_uint16> getAncestry() const;
    //     void setAncestry(const QVector<tu_uint16> &ancestry);
    //
    //     SideEffect validate(const Atom &atom) override;
    //     ValidatorComparisonType compare(ValidatorNode *other) const override;
    //     uint hash() const override;
    //
    //     ValidatorNode *clone() const override;
    //
    // private:
    //     QVector<tu_uint16> m_ancestry;
    // };

    class ValidateAnyOf : public ValidatorNode {

    public:
        ValidateAnyOf();
        explicit ValidateAnyOf(const std::vector<std::shared_ptr<ValidatorNode>> &anyOf);
        explicit ValidateAnyOf(std::vector<std::shared_ptr<ValidatorNode>> &&anyOf);
        explicit ValidateAnyOf(std::initializer_list<std::shared_ptr<ValidatorNode>> anyOf);

        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        std::shared_ptr<ValidatorNode> getNode(int index) const;
        std::vector<std::shared_ptr<ValidatorNode>>::const_iterator cbegin() const;
        std::vector<std::shared_ptr<ValidatorNode>>::const_iterator cend() const;
        int numNodes() const;

    private:
        std::vector<std::shared_ptr<ValidatorNode>> m_anyOf;
    };

    class ValidateAllOf : public ValidatorNode {

    public:
        ValidateAllOf();
        explicit ValidateAllOf(const std::vector<std::shared_ptr<ValidatorNode>> &allOf);
        explicit ValidateAllOf(std::vector<std::shared_ptr<ValidatorNode>> &&allOf);
        explicit ValidateAllOf(std::initializer_list<std::shared_ptr<ValidatorNode>> allOf);

        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        std::shared_ptr<ValidatorNode> getNode(int index) const;
        std::vector<std::shared_ptr<ValidatorNode>>::const_iterator cbegin() const;
        std::vector<std::shared_ptr<ValidatorNode>>::const_iterator cend() const;
        int numNodes() const;

    private:
        std::vector<std::shared_ptr<ValidatorNode>> m_allOf;
    };

    class ValidateNot : public ValidatorNode {

    public:
        ValidateNot();
        ValidateNot(std::shared_ptr<ValidatorNode> node);

        ValidatorNodeType getType() const override;
        ValidatorValueType getValueType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        std::shared_ptr<ValidatorNode> getNode() const;

    private:
        std::shared_ptr<ValidatorNode> m_node;
    };
}

#endif // TEMPO_CONFIG_VALIDATOR_TYPES_H
