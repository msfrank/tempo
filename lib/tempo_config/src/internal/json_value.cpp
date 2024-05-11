
#include <tempo_config/internal/json_value.h>

tempo_config::internal::JsonValue::JsonValue(std::nullptr_t n, const ConfigLocation &location)
    : m_value(ConfigNil(location))
{
}

tempo_config::internal::JsonValue::JsonValue(std::string_view value, const ConfigLocation &location)
    : m_value(ConfigValue(value, location))
{
}

tempo_config::internal::JsonNodeType
tempo_config::internal::JsonValue::getNodeType() const
{
    return JsonNodeType::Value;
}

tempo_config::ConfigNode
tempo_config::internal::JsonValue::getConfig() const
{
    return m_value;
}
