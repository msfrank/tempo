
#include <tempo_config/internal/lexer_error_listener.h>
#include <tempo_config/config_result.h>

void
tempo_config::internal::LexerErrorListener::syntaxError(
    antlr4::Recognizer *recognizer,
    antlr4::Token *offendingSymbol,
    size_t line,
    size_t charPositionInLine,
    const std::string &message,
    std::exception_ptr e)
{
    if (!e) {
        throw tempo_utils::StatusException(
            tempo_utils::Status(tempo_utils::StatusCode::kInternal, message));
    }

    try {
        std::rethrow_exception(e);
    } catch(antlr4::LexerNoViableAltException &ex) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kParseError, message));
    } catch(antlr4::RuntimeException &ex) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kConfigInvariant, message));
    }
}