
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
            ConfigStatus::forCondition(
                ConfigCondition::kParseError, message));

    // we know exception is empty due to the following conditions:
    //  - reporting an unwanted token which was resolved using single token deletion
    //  - reporting a missing token which was resolved using single token insertion
    //  - reporting ambiguity warnings
    if (!e) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kParseError, message));
    }

    // convert antlr4 exceptions into our own exceptions
    try {
        std::rethrow_exception(e);
    } catch(antlr4::FailedPredicateException &ex) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kParseError, message));
    } catch(antlr4::InputMismatchException &ex) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kParseError, message));
    } catch(antlr4::NoViableAltException &ex) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kParseError, message));
    } catch(antlr4::RuntimeException &ex) {
        throw tempo_utils::StatusException(
            ConfigStatus::forCondition(ConfigCondition::kConfigInvariant, message));
    }
}

void
tempo_config::internal::ParserErrorStrategy::reportUnwantedToken(antlr4::Parser *recognizer)
{
}

void
tempo_config::internal::ParserErrorStrategy::reportMissingToken(antlr4::Parser *recognizer)
{
}