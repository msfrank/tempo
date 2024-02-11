#ifndef TEMPO_CONFIG_BASE_CONVERSIONS_H
#define TEMPO_CONFIG_BASE_CONVERSIONS_H

#include <filesystem>

#include <tempo_utils/integer_types.h>
#include <tempo_utils/option_template.h>
#include <tempo_utils/url.h>

#include "abstract_config_parser.h"
#include "config_serde.h"
#include "config_types.h"
#include "config_result.h"

namespace tempo_config {

    class BooleanParser : public AbstractConfigParser<bool> {
    public:
        BooleanParser();
        BooleanParser(bool booleanDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, bool &b) const override;

    private:
        Option<bool> m_default;
    };

    class IntegerParser : public AbstractConfigParser<int> {
    public:
        IntegerParser();
        IntegerParser(int integerDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, int &i) const override;

    private:
        Option<int> m_default;
    };

    class LongParser : public AbstractConfigParser<tu_int64> {
    public:
        LongParser();
        LongParser(tu_int64 longDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, tu_int64 &i64) const override;

    private:
        Option<tu_int64> m_default;
    };

    class FloatParser : public AbstractConfigParser<double> {
    public:
        FloatParser();
        FloatParser(double doubleDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, double &dbl) const override;

    private:
        Option<double> m_default;
    };

    class StringParser : public AbstractConfigParser<std::string> {
    public:
        StringParser();
        StringParser(const std::string &stringDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, std::string &str) const override;

    private:
        Option<std::string> m_default;
    };

    class PathParser : public AbstractConfigParser<std::filesystem::path> {
    public:
        PathParser();
        PathParser(const std::filesystem::path &pathDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, std::filesystem::path &path) const override;

    private:
        Option<std::filesystem::path> m_default;
    };

    class UrlParser : public AbstractConfigParser<tempo_utils::Url> {
    public:
        UrlParser();
        UrlParser(const tempo_utils::Url &uriDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, tempo_utils::Url &uri) const override;

    private:
        Option<tempo_utils::Url> m_default;
    };

    class ConfigStringParser : public AbstractConfigParser<ConfigNode> {
    public:
        ConfigStringParser();
        ConfigStringParser(const ConfigNode &configDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, ConfigNode &value) const override;

    private:
        Option<ConfigNode> m_default;
    };

    class ConfigFileParser : public AbstractConfigParser<ConfigFile> {
    public:
        ConfigFileParser();
        ConfigFileParser(const ConfigNode &configDefault);
        tempo_utils::Status parseValue(const ConfigNode &node, ConfigFile &value) const override;

    private:
        Option<ConfigNode> m_default;
    };
}

#endif // TEMPO_CONFIG_BASE_CONVERSIONS_H