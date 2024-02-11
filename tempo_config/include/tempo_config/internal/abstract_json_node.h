#ifndef TEMPO_CONFIG_INTERNAL_ABSTRACT_JSON_NODE_H
#define TEMPO_CONFIG_INTERNAL_ABSTRACT_JSON_NODE_H

#include <tempo_config/config_types.h>

namespace tempo_config::internal {

    enum class JsonNodeType {
        Invalid,
        Value,
        Array,
        Object,
    };

    class AbstractJsonNode {
    public:
        virtual ~AbstractJsonNode() = default;

        virtual JsonNodeType getNodeType() const = 0;
        virtual ConfigNode getConfig() const = 0;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_ABSTRACT_JSON_NODE_H
