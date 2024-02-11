
#include <tempo_config/internal/config_listener.h>
#include <tempo_config/internal/json_array.h>
#include <tempo_config/internal/json_object.h>
#include <tempo_config/internal/json_value.h>
#include <tempo_utils/log_message.h>

tempo_config::internal::ConfigListener::ConfigListener(std::shared_ptr<ConfigSource> source)
    : m_source(source),
      m_root(nullptr)
{
}

std::shared_ptr<tempo_config::ConfigSource>
tempo_config::internal::ConfigListener::getSource() const
{
    return m_source;
}

tempo_config::ConfigNode
tempo_config::internal::ConfigListener::getConfig() const
{
    if (m_root == nullptr)
        return {};
    return m_root->getConfig();
}

inline tempo_config::ConfigLocation
antlr_ctx_to_config_location(std::shared_ptr<tempo_config::ConfigSource> source, antlr4::ParserRuleContext *ctx)
{
    TU_ASSERT (ctx != nullptr);
    auto *token = ctx->getStart();
    return tempo_config::ConfigLocation(source, token->getLine(), token->getCharPositionInLine(),
        token->getStartIndex(), token->getStopIndex() - token->getStartIndex());
}

void
tempo_config::internal::ConfigListener::exitNullLiteral(ConfigParser::NullLiteralContext *ctx)
{
    auto *node = new JsonValue(nullptr, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitTrueLiteral(ConfigParser::TrueLiteralContext *ctx)
{
    auto *node = new JsonValue("true", antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitFalseLiteral(ConfigParser::FalseLiteralContext *ctx)
{
    auto *node = new JsonValue("false", antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitInteger(ConfigParser::IntegerContext *ctx)
{
    auto number = ctx->DecimalInteger()->getText();
    auto *node = new JsonValue(number, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitFixedFloat(ConfigParser::FixedFloatContext *ctx)
{
    auto number = ctx->DecimalFixedFloat()->getText();
    auto *node = new JsonValue(number, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitScientificFloat(ConfigParser::ScientificFloatContext *ctx)
{
    auto number = ctx->DecimalScientificFloat()->getText();
    auto *node = new JsonValue(number, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::exitStringLiteral(ConfigParser::StringLiteralContext *ctx)
{
    auto quoted = ctx->StringLiteral()->getText();
    std::string str(quoted.cbegin() + 1, quoted.cend() - 1);
    auto *node = new JsonValue(str, antlr_ctx_to_config_location(m_source, ctx));
    push(node);
}

void
tempo_config::internal::ConfigListener::enterArray(ConfigParser::ArrayContext *ctx)
{
    auto *array = new JsonArray(antlr_ctx_to_config_location(m_source, ctx));
    push(array);
    pushArray(array);
}

void
tempo_config::internal::ConfigListener::exitElement(ConfigParser::ElementContext *ctx)
{
    auto *element = pop();
    TU_ASSERT (element != nullptr);

    auto *array = currentArray();
    TU_ASSERT (array != nullptr);

    array->appendElement(element);
}

void
tempo_config::internal::ConfigListener::exitArray(ConfigParser::ArrayContext *ctx)
{
    popArray();
}

void
tempo_config::internal::ConfigListener::enterObject(ConfigParser::ObjectContext *ctx)
{
    auto *object = new JsonObject(antlr_ctx_to_config_location(m_source, ctx));
    push(object);
    pushObject(object);
}

void
tempo_config::internal::ConfigListener::exitMember(ConfigParser::MemberContext *ctx)
{
    auto *value = pop();
    TU_ASSERT (value != nullptr);

    auto *object = currentObject();
    TU_ASSERT (object != nullptr);

    auto key = ctx->memberKey()->getText();
    TU_ASSERT (value != nullptr);
    object->putMember(std::string(key.cbegin() + 1, key.cend() - 1), value);
}

void
tempo_config::internal::ConfigListener::exitObject(ConfigParser::ObjectContext *ctx)
{
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