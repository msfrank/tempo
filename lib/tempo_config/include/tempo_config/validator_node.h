#ifndef TEMPO_CONFIG_VALIDATOR_TYPES_H
#define TEMPO_CONFIG_VALIDATOR_TYPES_H

#include <tempo_utils/status.h>

#include "config_types.h"

namespace tempo_config {

    enum class ValidatorNodeType {
        Invalid,
        Any,
        None,
        Const,
        Enum,
        Integer,
        Float,
        String,
        Seq,
        Map,
        Class,
        AnyOf,
        AllOf,
        Not
    };

    enum class ValidatorComparisonType {
        Invalid,
        Disjoint,
        Subset,
        Equal,
    };

    class ValidatorNode {

    public:
        virtual ~ValidatorNode() = default;

        virtual ValidatorNodeType getType() const = 0;
        virtual tempo_utils::Status validate(const ConfigNode &node) = 0;
    };

    class ValidateAny : public ValidatorNode {
    public:
        ValidateAny() = default;
        ValidatorNodeType getType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;
    };

    class ValidateNone : public ValidatorNode {
    public:
        ValidateNone() = default;
        ValidatorNodeType getType() const override;
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
        tempo_utils::Status validate(const ConfigNode &node) override;

        tu_int64 getMinimum() const;
        tu_int64 getMaximum() const;
        tu_int64 getMultipleOf() const;
        bool getMinExclusive() const;
        bool getMaxExclusive() const;

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
        tempo_utils::Status validate(const ConfigNode &node) override;

        tu_int32 getMinLength() const;
        tu_int32 getMaxLength() const;
        std::string getPattern() const;


    private:
        tu_int32 m_minLength;
        tu_int32 m_maxLength;
        std::string m_pattern;
    };

    class ValidateSeq : public ValidatorNode {

    public:
        ValidateSeq(
            tu_uint32 minItems = 0,
            tu_uint32 maxItems = std::numeric_limits<tu_uint32>::max(),
            bool unique = false,
            std::shared_ptr<ValidatorNode> items = {},
            std::shared_ptr<ValidatorNode> contains = {});

        ValidatorNodeType getType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        tu_uint32 getMinItems() const;
        tu_uint32 getMaxItems() const;
        bool getUnique() const;
        std::shared_ptr<ValidatorNode> getItems() const;
        std::shared_ptr<ValidatorNode> getContains() const;


    private:
        tu_uint32 m_minItems;
        tu_uint32 m_maxItems;
        bool m_unique;
        std::shared_ptr<ValidatorNode> m_items;
        std::shared_ptr<ValidatorNode> m_contains;
    };

    class ValidateMap : public ValidatorNode {

    public:
        ValidateMap(
            tu_uint32 minProperties = 0,
            tu_uint32 maxProperties = std::numeric_limits<tu_uint32>::max(),
            const absl::flat_hash_set<std::string> &required = {},
            const absl::flat_hash_map<std::string,std::shared_ptr<ValidatorNode>> &properties = {},
            std::shared_ptr<ValidatorNode> additional = {});

        ValidatorNodeType getType() const override;
        tempo_utils::Status validate(const ConfigNode &node) override;

        tu_uint32 getMinProperties() const;
        tu_uint32 getMaxProperties() const;

        bool hasRequired(std::string_view name) const;
        absl::flat_hash_set<std::string>::const_iterator requiredBegin() const;
        absl::flat_hash_set<std::string>::const_iterator requiredEnd() const;
        int numRequired() const;

        bool hasProperty(std::string_view name) const;
        std::shared_ptr<ValidatorNode> getProperty(std::string_view name) const;
        absl::flat_hash_map<std::string,std::shared_ptr<ValidatorNode>>::const_iterator propertiesBegin() const;
        absl::flat_hash_map<std::string,std::shared_ptr<ValidatorNode>>::const_iterator propertiesEnd() const;
        int numProperties() const;

    private:
        tu_uint32 m_minProperties;
        tu_uint32 m_maxProperties;
        absl::flat_hash_set<std::string> m_required;
        absl::flat_hash_map<std::string,std::shared_ptr<ValidatorNode>> m_properties;
        std::shared_ptr<ValidatorNode> m_additional;
    };

    class ValidateAnyOf : public ValidatorNode {

    public:
        ValidateAnyOf();
        explicit ValidateAnyOf(const std::vector<std::shared_ptr<ValidatorNode>> &anyOf);
        explicit ValidateAnyOf(std::vector<std::shared_ptr<ValidatorNode>> &&anyOf);
        explicit ValidateAnyOf(std::initializer_list<std::shared_ptr<ValidatorNode>> anyOf);

        ValidatorNodeType getType() const override;
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
        tempo_utils::Status validate(const ConfigNode &node) override;

        std::shared_ptr<ValidatorNode> getNode() const;

    private:
        std::shared_ptr<ValidatorNode> m_node;
    };
}

#endif // TEMPO_CONFIG_VALIDATOR_TYPES_H
