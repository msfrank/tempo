#ifndef TEMPO_CONFIG_INTERNAL_JSON_OBJECT_H
#define TEMPO_CONFIG_INTERNAL_JSON_OBJECT_H

#include <string>

#include <absl/container/flat_hash_map.h>

#include <tempo_utils/integer_types.h>

#include "abstract_json_node.h"

namespace tempo_config::internal {

    class JsonObject : public AbstractJsonNode {
    public:
        explicit JsonObject(const ConfigLocation &location);
        ~JsonObject();

        JsonNodeType getNodeType() const override;
        ConfigNode getConfig() const override;

        bool hasMember(std::string_view key) const;
        AbstractJsonNode *getMember(std::string_view key) const;
        void putMember(std::string_view key, AbstractJsonNode *value);
        absl::flat_hash_map<std::string,AbstractJsonNode *>::const_iterator cbegin() const;
        absl::flat_hash_map<std::string,AbstractJsonNode *>::const_iterator cend() const;
        int numMembers() const;

    private:
        ConfigLocation m_location;
        absl::flat_hash_map<std::string,AbstractJsonNode *> m_members;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_JSON_OBJECT_H