#ifndef TEMPO_CONFIG_BASE_CONVERSIONS_H
#define TEMPO_CONFIG_BASE_CONVERSIONS_H

#include <filesystem>

#include <tempo_utils/integer_types.h>
#include <tempo_utils/option_template.h>
#include <tempo_utils/url.h>

#include "abstract_converter.h"
#include "config_types.h"
#include "config_result.h"

namespace tempo_config {

    class BooleanParser : public AbstractConverter<bool> {
    public:
        BooleanParser();
        BooleanParser(bool booleanDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, bool &b) const override;

    private:
        Option<bool> m_default;
    };

    class IntegerParser : public AbstractConverter<int> {
    public:
        IntegerParser();
        IntegerParser(int integerDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, int &i) const override;

    private:
        Option<int> m_default;
    };

    class LongParser : public AbstractConverter<tu_int64> {
    public:
        LongParser();
        LongParser(tu_int64 longDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, tu_int64 &i64) const override;

    private:
        Option<tu_int64> m_default;
    };

    class FloatParser : public AbstractConverter<double> {
    public:
        FloatParser();
        FloatParser(double doubleDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, double &dbl) const override;

    private:
        Option<double> m_default;
    };

    class StringParser : public AbstractConverter<std::string> {
    public:
        StringParser();
        StringParser(const std::string &stringDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, std::string &str) const override;

    private:
        Option<std::string> m_default;
    };

    class PathParser : public AbstractConverter<std::filesystem::path> {
    public:
        PathParser();
        PathParser(const std::filesystem::path &pathDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, std::filesystem::path &path) const override;

    private:
        Option<std::filesystem::path> m_default;
    };

    class UrlParser : public AbstractConverter<tempo_utils::Url> {
    public:
        UrlParser();
        UrlParser(const tempo_utils::Url &urlDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, tempo_utils::Url &url) const override;

    private:
        Option<tempo_utils::Url> m_default;
    };

    class UrlPathParser : public AbstractConverter<tempo_utils::UrlPath> {
    public:
        UrlPathParser();
        UrlPathParser(const tempo_utils::UrlPath &pathDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, tempo_utils::UrlPath &path) const override;

    private:
        Option<tempo_utils::UrlPath> m_default;
    };

    class ConfigStringParser : public AbstractConverter<ConfigNode> {
    public:
        ConfigStringParser();
        ConfigStringParser(const ConfigNode &configDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, ConfigNode &value) const override;

    private:
        Option<ConfigNode> m_default;
    };

    class ConfigFileParser : public AbstractConverter<ConfigFile> {
    public:
        ConfigFileParser();
        ConfigFileParser(const ConfigNode &configDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, ConfigFile &value) const override;

    private:
        Option<ConfigNode> m_default;
    };
}

#endif // TEMPO_CONFIG_BASE_CONVERSIONS_H