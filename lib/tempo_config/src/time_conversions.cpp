
#include <tempo_config/config_result.h>
#include <tempo_config/time_conversions.h>

tempo_config::TimeParser::TimeParser(std::string_view timeFormat)
    : m_timeFormat(timeFormat)
{
}

tempo_config::TimeParser::TimeParser(std::string_view timeFormat, const absl::Time &timeDefault)
    : TimeParser(timeFormat)
{
    m_default = Option(timeDefault);
}

tempo_utils::Status
tempo_config::TimeParser::convertValue(const ConfigNode &node, absl::Time &time) const
{
    if (node.isNil()) {
        if (m_default.isEmpty())
            return ConfigStatus::forCondition(ConfigCondition::kMissingValue,
                "missing required time value");
        time = m_default.getValue();
        return {};
    }

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));
    auto value = node.toValue().getValue();

    std::string err;
    if (!absl::ParseTime(m_timeFormat, value, &time, &err))
        return ConfigStatus::forCondition(ConfigCondition::kParseError,
            "value '{}' cannot be converted to time: {}", value, err);
    return {};
}

tempo_config::DurationParser::DurationParser()
{
}

tempo_config::DurationParser::DurationParser(const absl::Duration &durationDefault)
    : m_default(durationDefault)
{
}

tempo_utils::Status
tempo_config::DurationParser::convertValue(const ConfigNode &node, absl::Duration &duration) const
{
    if (node.isNil()) {
        if (m_default.isEmpty())
            return ConfigStatus::forCondition(ConfigCondition::kMissingValue,
                "missing required duration value");
        duration = m_default.getValue();
        return {};
    }

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));
    auto value = node.toValue().getValue();

    std::string err;
    if (!absl::ParseDuration(value, &duration))
        return ConfigStatus::forCondition(ConfigCondition::kParseError,
            "value '{}' is not a valid duration", value);
    return {};
}

tempo_config::TimeZoneParser::TimeZoneParser()
{
}

tempo_config::TimeZoneParser::TimeZoneParser(const absl::TimeZone &tzDefault)
    : m_default(tzDefault)
{
}

tempo_utils::Status
tempo_config::TimeZoneParser::convertValue(const ConfigNode &node, absl::TimeZone &tz) const
{
    if (node.isNil()) {
        if (m_default.isEmpty())
            return ConfigStatus::forCondition(ConfigCondition::kMissingValue,
                "missing required timezone value");
        tz = m_default.getValue();
        return {};
    }

    if (node.getNodeType() != ConfigNodeType::kValue)
        return ConfigStatus::forCondition(ConfigCondition::kWrongType,
            "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));
    auto value = node.toValue().getValue();

    std::string err;
    if (!absl::LoadTimeZone(value, &tz))
        return ConfigStatus::forCondition(ConfigCondition::kParseError,
            "value '{}' is not a valid timezone", value);
    return {};
}
