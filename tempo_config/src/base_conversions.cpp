
#include <absl/strings/match.h>
#include <absl/strings/numbers.h>

#include <tempo_config/base_conversions.h>
#include <tempo_config/config_serde.h>

tempo_config::BooleanParser::BooleanParser()
{
}

tempo_config::BooleanParser::BooleanParser(bool booleanDefault)
    : m_default(booleanDefault)
{
}

#define SET_VALUE_AND_RETURN_OK(dst,src) \
    do { dst = src; return ConfigStatus::ok(); } while (0)

tempo_utils::Status
tempo_config::BooleanParser::parseValue(const ConfigNode &node, bool &b) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(b, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();

    if (absl::EqualsIgnoreCase(value, "true"))
        SET_VALUE_AND_RETURN_OK(b, true);

    if (absl::EqualsIgnoreCase(value, "false"))
        SET_VALUE_AND_RETURN_OK(b, false);

    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "value '{}' cannot be converted to bool", value);
}

tempo_config::IntegerParser::IntegerParser()
{
}

tempo_config::IntegerParser::IntegerParser(int integerDefault)
    : m_default(integerDefault)
{
}

tempo_utils::Status
tempo_config::IntegerParser::parseValue(const ConfigNode &node, int &i) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(i, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();

    int i__;
    if (absl::SimpleAtoi(value, &i__))
        SET_VALUE_AND_RETURN_OK(i, i__);

    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "value '{}' cannot be converted to int", value);
}

tempo_config::LongParser::LongParser()
{
}

tempo_config::LongParser::LongParser(int64_t longDefault)
    : m_default(longDefault)
{
}

tempo_utils::Status
tempo_config::LongParser::parseValue(const ConfigNode &node, tu_int64 &i64) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(i64, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();

    int64_t i64__;
    if (absl::SimpleAtoi(value, &i64__))
        SET_VALUE_AND_RETURN_OK(i64, i64__);

    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "value '{}' cannot be converted to int64", value);
}

tempo_config::FloatParser::FloatParser()
{
}

tempo_config::FloatParser::FloatParser(double floatDefault)
    : m_default(floatDefault)
{
}

tempo_utils::Status
tempo_config::FloatParser::parseValue(const ConfigNode &node, double &dbl) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(dbl, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();

    double dbl__;
    if (absl::SimpleAtod(value, &dbl__))
        SET_VALUE_AND_RETURN_OK(dbl, dbl__);

    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "value '{}' cannot be converted to double", value);
}

tempo_config::StringParser::StringParser()
{
}

tempo_config::StringParser::StringParser(const std::string &stringDefault)
    : m_default(stringDefault)
{
}

tempo_utils::Status
tempo_config::StringParser::parseValue(const ConfigNode &node, std::string &str) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(str, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    SET_VALUE_AND_RETURN_OK(str, node.toValue().getValue());
}

tempo_config::PathParser::PathParser()
{
}

tempo_config::PathParser::PathParser(const std::filesystem::path &pathDefault)
    : m_default(pathDefault)
{
}

tempo_utils::Status
tempo_config::PathParser::parseValue(const ConfigNode &node, std::filesystem::path &path) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(path, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();
    std::filesystem::path path__(value, std::filesystem::path::format::native_format);
    if (!path__.empty())
        SET_VALUE_AND_RETURN_OK(path, path__);

    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "value '{}' cannot be converted to path", value);
}

tempo_config::UrlParser::UrlParser()
{
}

tempo_config::UrlParser::UrlParser(const tempo_utils::Url &uriDefault)
    : m_default(uriDefault)
{
}

tempo_utils::Status
tempo_config::UrlParser::parseValue(const ConfigNode &node, tempo_utils::Url &uri) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(uri, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();
    auto uri__ = tempo_utils::Url::fromString(value);
    if (uri__.isValid())
        SET_VALUE_AND_RETURN_OK(uri, uri__);

    return ConfigStatus::forCondition(ConfigCondition::kParseError,
        "value '{}' cannot be converted to Url", value);
}

tempo_config::ConfigStringParser::ConfigStringParser()
{
}

tempo_config::ConfigStringParser::ConfigStringParser(const ConfigNode &configDefault)
    : m_default(configDefault)
{
}

tempo_utils::Status
tempo_config::ConfigStringParser::parseValue(const ConfigNode &node, ConfigNode &n) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(n, m_default.getValue());

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();
    auto readConfigResult = read_config_string(value);
    if (readConfigResult.isResult())
        SET_VALUE_AND_RETURN_OK(n, readConfigResult.getResult());

    return readConfigResult.getStatus();
}

tempo_config::ConfigFileParser::ConfigFileParser()
{
}

tempo_config::ConfigFileParser::ConfigFileParser(const ConfigNode &configDefault)
    : m_default(configDefault)
{
}

tempo_utils::Status
tempo_config::ConfigFileParser::parseValue(const ConfigNode &node, ConfigFile &f) const
{
    if (node.isNil() && !m_default.isEmpty())
        SET_VALUE_AND_RETURN_OK(f, ConfigFile({}, m_default.getValue()));

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

    auto value = node.toValue().getValue();
    std::filesystem::path path(value, std::filesystem::path::format::native_format);
    auto readConfigResult = read_config_file(path);
    if (readConfigResult.isResult())
        SET_VALUE_AND_RETURN_OK(f, ConfigFile(path, readConfigResult.getResult()));

    return readConfigResult.getStatus();
}