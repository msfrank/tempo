#ifndef TEMPO_CONFIG_INTERNAL_CONFIG_LISTENER_H
#define TEMPO_CONFIG_INTERNAL_CONFIG_LISTENER_H

#include <string>

#include <absl/container/flat_hash_map.h>

#include <tempo_config/config_types.h>

#include "ConfigParserBaseListener.h"
#include "abstract_json_node.h"
#include "json_array.h"
#include "json_object.h"

namespace tempo_config::internal {

    class ConfigListener : public ConfigParserBaseListener {

    public:
        explicit ConfigListener(std::shared_ptr<ConfigSource> source);

        std::shared_ptr<ConfigSource> getSource() const;
        ConfigNode getConfig() const;

        void exitNullLiteral(ConfigParser::NullLiteralContext *ctx) override;
        void exitTrueLiteral(ConfigParser::TrueLiteralContext *ctx) override;
        void exitFalseLiteral(ConfigParser::FalseLiteralContext *ctx) override;
        void exitInteger(ConfigParser::IntegerContext *ctx) override;
        void exitFixedFloat(ConfigParser::FixedFloatContext *ctx) override;
        void exitScientificFloat(ConfigParser::ScientificFloatContext *ctx) override;
        void exitStringLiteral(ConfigParser::StringLiteralContext *ctx) override;

        void enterArray(ConfigParser::ArrayContext *ctx) override;
        void exitElement(ConfigParser::ElementContext *ctx) override;
        void exitArray(ConfigParser::ArrayContext *ctx) override;

        void enterObject(ConfigParser::ObjectContext *ctx) override;
        void exitMember(ConfigParser::MemberContext *ctx) override;
        void exitObject(ConfigParser::ObjectContext *ctx) override;

        bool isEmpty() const;
        AbstractJsonNode *peek(int offset = 0) const;
        void push(AbstractJsonNode *node);
        AbstractJsonNode *pop();

        JsonArray *currentArray() const;
        void pushArray(JsonArray *array);
        JsonArray *popArray();

        JsonObject *currentObject() const;
        void pushObject(JsonObject *array);
        JsonObject *popObject();

    private:
        std::shared_ptr<ConfigSource> m_source;
        AbstractJsonNode *m_root;
        std::vector<AbstractJsonNode *> m_stack;
        std::vector<JsonArray *> m_arrays;
        std::vector<JsonObject *> m_objects;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_CONFIG_LISTENER_H
