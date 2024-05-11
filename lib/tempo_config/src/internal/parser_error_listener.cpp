
#include <tempo_config/internal/parser_error_listener.h>
#include <tempo_config/config_result.h>

void
tempo_config::internal::ParserErrorListener::syntaxError(
    antlr4::Recognizer *recognizer,
    antlr4::Token *offendingSymbol,
    size_t line,
    size_t charPositionInLine,
    const std::string &message,
    std::exception_ptr e)
{
    // FIXME: could this ever be nullptr?
    if (!offendingSymbol)
        throw tempo_utils::StatusException(
            tempo_utils::Status(tempo_utils::StatusCode::kInternal, message));

    // if match failed due to end-of-file, then throw IncompleteModuleException
    if (offendingSymbol->getType() == antlr4::Token::EOF)
        throw tempo_utils::StatusException(
            tempo_config::ConfigStatus::forCondition(
                tempo_config::ConfigCondition::kParseError, message));

    // FIXME: when does antlr4 raise a syntax error with an empty exception?
    if (!e)
        throw tempo_utils::StatusException(
            tempo_config::ConfigStatus::forCondition(
                tempo_config::ConfigCondition::kConfigInvariant, message));

    // convert antlr4 exceptions into our own exceptions
    try {
        std::rethrow_exception(e);
    } catch(antlr4::FailedPredicateException &ex) {
        throw tempo_utils::StatusException(
            tempo_config::ConfigStatus::forCondition(
                tempo_config::ConfigCondition::kParseError, message));
    } catch(antlr4::InputMismatchException &ex) {
        throw tempo_utils::StatusException(
            tempo_config::ConfigStatus::forCondition(
                tempo_config::ConfigCondition::kParseError, message));
    } catch(antlr4::NoViableAltException &ex) {
        throw tempo_utils::StatusException(
            tempo_config::ConfigStatus::forCondition(
                tempo_config::ConfigCondition::kParseError, message));
    } catch(antlr4::RuntimeException &ex) {
        throw tempo_utils::StatusException(
            tempo_config::ConfigStatus::forCondition(
                tempo_config::ConfigCondition::kConfigInvariant, message));
    }
}

void
tempo_config::internal::ParserErrorStrategy::recover(antlr4::Parser *recognizer, std::exception_ptr e)
{
    throw tempo_utils::StatusException(
        tempo_config::ConfigStatus::forCondition(
            tempo_config::ConfigCondition::kParseError, "parse failure on recover"));
}

antlr4::Token *
tempo_config::internal::ParserErrorStrategy::recoverInline(antlr4::Parser *recognizer)
{
    throw tempo_utils::StatusException(
        tempo_config::ConfigStatus::forCondition(
            tempo_config::ConfigCondition::kParseError, "parse failure on recoverInline"));
}

void
tempo_config::internal::ParserErrorStrategy::sync(antlr4::Parser *recognizer)
{
}
