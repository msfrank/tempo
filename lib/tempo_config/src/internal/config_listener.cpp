
#include <absl/strings/substitute.h>

#include <tempo_config/config_result.h>
#include <tempo_config/internal/config_listener.h>
#include <tempo_config/internal/json_array.h>
#include <tempo_config/internal/json_object.h>
#include <tempo_config/internal/json_value.h>
#include <tempo_tracing/current_scope.h>
#include <tempo_tracing/enter_scope.h>
#include <tempo_tracing/exit_scope.h>
#include <tempo_tracing/span_log.h>
#include <tempo_tracing/tracing_schema.h>
#include <tempo_utils/log_message.h>

tempo_config::internal::ConfigListener::ConfigListener(
    std::shared_ptr<ConfigSource> source,
    std::shared_ptr<tempo_tracing::TraceContext> context)
    : m_source(std::move(source)),
      m_context(std::move(context)),
      m_root(nullptr)
{
    TU_ASSERT (m_source != nullptr);
    TU_ASSERT (m_context != nullptr);
}

std::shared_ptr<tempo_config::ConfigSource>
tempo_config::internal::ConfigListener::getSource() const
{
    return m_source;
}

void
tempo_config::internal::ConfigListener::logErrorOrThrow(
    size_t lineNr,
    size_t columnNr,
    const std::string &message)
{
    // if this is the first error seen then set status
    if (m_status.isOk()) {
        auto fullMessage = absl::Substitute(
            "parse error at $0:$1: $2", lineNr, columnNr, message);
        m_status = ConfigStatus::forCondition(
            ConfigCondition::kParseError, fullMessage);
    }

    tempo_tracing::CurrentScope scope;
    scope.putTag(tempo_tracing::kOpentracingError, true);
    auto log = scope.appendLog(absl::Now(), tempo_tracing::LogSeverity::kError);
    log->putField(tempo_tracing::kTempoTracingLineNumber, (tu_uint64) lineNr);
    log->putField(tempo_tracing::kTempoTracingColumnNumber, (tu_uint64) columnNr);
    log->putField(tempo_tracing::kOpentracingMessage, message);
}

bool
tempo_config::internal::ConfigListener::hasError() const
{
    return m_status.notOk();
}

inline tempo_config::ConfigLocation
antlr_ctx_to_config_location(std::shared_ptr<tempo_config::ConfigSource> source, antlr4::ParserRuleContext *ctx)
{
    TU_ASSERT (ctx != nullptr);
    auto *token = ctx->getStart();
    return tempo_config::ConfigLocation(std::move(source), token->getLine(), token->getCharPositionInLine(),
        token->getStartIndex(), token->getStopIndex() - token->getStartIndex());
}

#define IGNORE_RULE_IF_HAS_ERROR   do { if (hasError()) return; } while (0);

void
tempo_config::internal::ConfigListener::enterRoot(tcf1::ConfigParser::RootContext *ctx)
{
    tempo_tracing::EnterScope scope("tempo_config::ConfigListener::enterRoot");
}

void
tempo_config::internal::ConfigListener::exitRoot(tcf1::ConfigParser::RootContext *ctx)
{
    tempo_tracing::ExitScope scope;
}

void
tempo_config::internal::ConfigListener::exitNullLiteral(tcf1::ConfigParser::NullLiteralContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto *node = new JsonValue(nullptr, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitTrueLiteral(tcf1::ConfigParser::TrueLiteralContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto *node = new JsonValue("true", antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitFalseLiteral(tcf1::ConfigParser::FalseLiteralContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto *node = new JsonValue("false", antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitInteger(tcf1::ConfigParser::IntegerContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto number = ctx->DecimalInteger()->getText();
    auto *node = new JsonValue(number, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitFixedFloat(tcf1::ConfigParser::FixedFloatContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto number = ctx->DecimalFixedFloat()->getText();
    auto *node = new JsonValue(number, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitScientificFloat(tcf1::ConfigParser::ScientificFloatContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto number = ctx->DecimalScientificFloat()->getText();
    auto *node = new JsonValue(number, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitStringLiteral(tcf1::ConfigParser::StringLiteralContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto quoted = ctx->StringLiteral()->getText();
    std::string str(quoted.cbegin() + 1, quoted.cend() - 1);
    auto *node = new JsonValue(str, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::enterArray(tcf1::ConfigParser::ArrayContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto *array = new JsonArray(antlr_ctx_to_config_location(m_source, ctx));
    push(array);
    pushArray(array);
}

void
tempo_config::internal::ConfigListener::exitElement(tcf1::ConfigParser::ElementContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR

    auto *element = pop();
    TU_ASSERT (element != nullptr);

    auto *array = currentArray();
    TU_ASSERT (array != nullptr);

    array->appendElement(element);
}

void
tempo_config::internal::ConfigListener::exitArray(tcf1::ConfigParser::ArrayContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    popArray();
}

void
tempo_config::internal::ConfigListener::enterObject(tcf1::ConfigParser::ObjectContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    auto *object = new JsonObject(antlr_ctx_to_config_location(m_source, ctx));
    push(object);
    pushObject(object);
}

void
tempo_config::internal::ConfigListener::exitMember(tcf1::ConfigParser::MemberContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR

    auto *value = pop();
    TU_ASSERT (value != nullptr);

    auto *object = currentObject();
    TU_ASSERT (object != nullptr);

    auto key = ctx->memberKey()->getText();
    TU_ASSERT (value != nullptr);
    object->putMember(std::string(key.cbegin() + 1, key.cend() - 1), value);
}

void
tempo_config::internal::ConfigListener::exitObject(tcf1::ConfigParser::ObjectContext *ctx)
{
    IGNORE_RULE_IF_HAS_ERROR
    popObject();
}

bool
tempo_config::internal::ConfigListener::isEmpty() const
{
    return m_stack.empty();
}

tempo_config::internal::AbstractJsonNode *
tempo_config::internal::ConfigListener::peek(int offset) const
{
    TU_ASSERT (offset >= 0);

    if (m_stack.empty())
        return nullptr;
    auto index = m_stack.size() - offset - 1;
    if (index < 0)
        return nullptr;

    return m_stack.at(index);
}

void
tempo_config::internal::ConfigListener::push(AbstractJsonNode *node)
{
    if (m_root == nullptr) {
        m_root = node;
    }
    m_stack.push_back(node);
}

tempo_config::internal::AbstractJsonNode *
tempo_config::internal::ConfigListener::pop()
{
    if (m_stack.empty())
        return nullptr;

    auto *top = m_stack.back();
    m_stack.pop_back();
    if (m_stack.empty()) {
        m_root = nullptr;
    }

    return top;
}

tempo_config::internal::JsonArray *
tempo_config::internal::ConfigListener::currentArray() const
{
    if (!m_arrays.empty())
        return m_arrays.back();
    return nullptr;
}

void
tempo_config::internal::ConfigListener::pushArray(JsonArray *array)
{
    TU_ASSERT (array != nullptr);
    m_arrays.push_back(array);
}

tempo_config::internal::JsonArray *
tempo_config::internal::ConfigListener::popArray()
{
    TU_ASSERT (!m_arrays.empty());
    auto *array = m_arrays.back();
    m_arrays.pop_back();
    return array;
}

tempo_config::internal::JsonObject *
tempo_config::internal::ConfigListener::currentObject() const
{
    if (!m_objects.empty())
        return m_objects.back();
    return nullptr;
}

void
tempo_config::internal::ConfigListener::pushObject(JsonObject *object)
{
    TU_ASSERT (object != nullptr);
    m_objects.push_back(object);
}

tempo_config::internal::JsonObject *
tempo_config::internal::ConfigListener::popObject()
{
    TU_ASSERT (!m_objects.empty());
    auto *object = m_objects.back();
    m_objects.pop_back();
    return object;
}

tempo_utils::Result<tempo_config::ConfigNode>
tempo_config::internal::ConfigListener::toConfig() const
{
    TU_RETURN_IF_NOT_OK (m_status);
    if (m_root == nullptr)
        return ConfigNode{};
    return m_root->getConfig();
}