#ifndef TEMPO_CONFIG_INTERNAL_JSON_ARRAY_H
#define TEMPO_CONFIG_INTERNAL_JSON_ARRAY_H

#include <vector>

#include <tempo_utils/integer_types.h>

#include "abstract_json_node.h"

namespace tempo_config::internal {

    class JsonArray : public AbstractJsonNode {
    public:
        explicit JsonArray(const ConfigLocation &location);
        ~JsonArray();

        JsonNodeType getNodeType() const override;
        ConfigNode getConfig() const override;

        AbstractJsonNode *getElement(int index) const;
        void prependElement(AbstractJsonNode *element);
        void appendElement(AbstractJsonNode *element);
        std::vector<AbstractJsonNode *>::const_iterator cbegin() const;
        std::vector<AbstractJsonNode *>::const_iterator cend() const;
        int numElements() const;

    private:
        ConfigLocation m_location;
        std::vector<AbstractJsonNode *> m_elements;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_JSON_ARRAY_H
