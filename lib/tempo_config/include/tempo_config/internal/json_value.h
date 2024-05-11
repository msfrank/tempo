#ifndef TEMPO_CONFIG_INTERNAL_JSON_VALUE_H
#define TEMPO_CONFIG_INTERNAL_JSON_VALUE_H

#include <cstddef>

#include <tempo_config/config_types.h>
#include <tempo_utils/integer_types.h>

#include "abstract_json_node.h"

namespace tempo_config::internal {

    class JsonValue : public AbstractJsonNode {
    public:
        explicit JsonValue(std::nullptr_t n, const ConfigLocation &location);
        explicit JsonValue(std::string_view value, const ConfigLocation &location);

        JsonNodeType getNodeType() const override;
        ConfigNode getConfig() const override;

    private:
        ConfigNode m_value;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_JSON_VALUE_H
