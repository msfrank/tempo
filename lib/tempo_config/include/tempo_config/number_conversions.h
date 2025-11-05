#ifndef TEMPO_CONFIG_NUMBER_CONVERSIONS_H
#define TEMPO_CONFIG_NUMBER_CONVERSIONS_H

#include "abstract_converter.h"
#include "config_result.h"

namespace tempo_config {

    template<typename IntegralType,
        typename = std::enable_if<std::is_integral<IntegralType>::value>::type>
    class IntegralNumberParser : public AbstractConverter<IntegralType> {
    public:
        IntegralNumberParser() = default;
        explicit IntegralNumberParser(IntegralType defaultNumber) : m_default(defaultNumber) {}

        tempo_utils::Status convertValue(const ConfigNode &node, IntegralType &num) const override
        {
            if (node.isNil()) {
                if (m_default.isEmpty())
                    return ConfigStatus::forCondition(ConfigCondition::kMissingValue,
                        "missing required integer value");
                num = m_default.getValue();
                return {};
            }

            if (node.getNodeType() != ConfigNodeType::kValue)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Value node but found {}",
                    config_node_type_to_string(node.getNodeType()));

            auto value = node.toValue().getValue();

            using ConvertedType = std::conditional<
                std::is_signed<IntegralType>::value, tu_int64, tu_uint64>::type;
            ConvertedType convertedNumber;

            if (!absl::SimpleAtoi(value, &convertedNumber))
                return ConfigStatus::forCondition(ConfigCondition::kParseError,
                    "value '{}' cannot be converted to an integer", value);

            if (convertedNumber < std::numeric_limits<IntegralType>::min()
                || convertedNumber > std::numeric_limits<IntegralType>::max())
                return ConfigStatus::forCondition(ConfigCondition::kParseError,
                    "value '{}' is out of range", value);

            num = static_cast<IntegralType>(convertedNumber);

            return {};
        }

    private:
        Option<IntegralType> m_default;
    };

    class Int64Parser : public IntegralNumberParser<tu_int64> {
    public:
        Int64Parser() = default;
        explicit Int64Parser(tu_int64 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };

    class Int32Parser : public IntegralNumberParser<tu_int32> {
    public:
        Int32Parser() = default;
        explicit Int32Parser(tu_int32 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };

    class Int16Parser : public IntegralNumberParser<tu_int16> {
    public:
        Int16Parser() = default;
        explicit Int16Parser(tu_int16 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };

    class Int8Parser : public IntegralNumberParser<tu_int8> {
    public:
        Int8Parser() = default;
        explicit Int8Parser(tu_int8 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };


    class UInt64Parser : public IntegralNumberParser<tu_uint64> {
    public:
        UInt64Parser() = default;
        explicit UInt64Parser(tu_uint64 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };

    class UInt32Parser : public IntegralNumberParser<tu_uint32> {
    public:
        UInt32Parser() = default;
        explicit UInt32Parser(tu_uint32 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };

    class UInt16Parser : public IntegralNumberParser<tu_uint16> {
    public:
        UInt16Parser() = default;
        explicit UInt16Parser(tu_uint16 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };

    class UInt8Parser : public IntegralNumberParser<tu_uint8> {
    public:
        UInt8Parser() = default;
        explicit UInt8Parser(tu_uint8 defaultNumber): IntegralNumberParser(defaultNumber) {}
    };
}

#endif // TEMPO_CONFIG_NUMBER_CONVERSIONS_H