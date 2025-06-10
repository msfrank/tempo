#ifndef TEMPO_CONFIG_INTERNAL_CONFIG_LISTENER_H
#define TEMPO_CONFIG_INTERNAL_CONFIG_LISTENER_H

#include <tempo_tracing/tempo_spanset.h>
#include <tempo_tracing/trace_context.h>

#include "ConfigParserBaseListener.h"

#include "../config_types.h"

#include "abstract_json_node.h"
#include "json_array.h"
#include "json_object.h"

namespace tempo_config::internal {

    class ConfigListener : public tcf1::ConfigParserBaseListener {

    public:
        ConfigListener(std::shared_ptr<ConfigSource> source, std::shared_ptr<tempo_tracing::TraceContext> context);

        std::shared_ptr<ConfigSource> getSource() const;

        void logErrorOrThrow(
            size_t lineNr,
            size_t columnNr,
            const std::string &message);

        bool hasError() const;

        void enterRoot(tcf1::ConfigParser::RootContext *ctx) override;
        void exitRoot(tcf1::ConfigParser::RootContext *ctx) override;

        void exitNullLiteral(tcf1::ConfigParser::NullLiteralContext *ctx) override;
        void exitTrueLiteral(tcf1::ConfigParser::TrueLiteralContext *ctx) override;
        void exitFalseLiteral(tcf1::ConfigParser::FalseLiteralContext *ctx) override;
        void exitInteger(tcf1::ConfigParser::IntegerContext *ctx) override;
        void exitFixedFloat(tcf1::ConfigParser::FixedFloatContext *ctx) override;
        void exitScientificFloat(tcf1::ConfigParser::ScientificFloatContext *ctx) override;
        void exitStringLiteral(tcf1::ConfigParser::StringLiteralContext *ctx) override;

        void enterArray(tcf1::ConfigParser::ArrayContext *ctx) override;
        void exitElement(tcf1::ConfigParser::ElementContext *ctx) override;
        void exitArray(tcf1::ConfigParser::ArrayContext *ctx) override;

        void enterObject(tcf1::ConfigParser::ObjectContext *ctx) override;
        void exitMember(tcf1::ConfigParser::MemberContext *ctx) override;
        void exitObject(tcf1::ConfigParser::ObjectContext *ctx) override;

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

        tempo_utils::Result<ConfigNode> toConfig() const;

    private:
        std::shared_ptr<ConfigSource> m_source;
        std::shared_ptr<tempo_tracing::TraceContext> m_context;

        AbstractJsonNode *m_root;
        std::vector<AbstractJsonNode *> m_stack;
        std::vector<JsonArray *> m_arrays;
        std::vector<JsonObject *> m_objects;
        tempo_utils::Status m_status;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_CONFIG_LISTENER_H
