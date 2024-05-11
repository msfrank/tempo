#ifndef TEMPO_CONFIG_INTERNAL_PARSER_ERROR_LISTENER_H
#define TEMPO_CONFIG_INTERNAL_PARSER_ERROR_LISTENER_H

#include <memory>

#include <antlr4-runtime.h>

namespace tempo_config::internal {

    class ParserErrorListener : public antlr4::BaseErrorListener {
    public:
        void syntaxError(
            antlr4::Recognizer *recognizer,
            antlr4::Token *offendingSymbol,
            size_t line,
            size_t charPositionInLine,
            const std::string &message,
            std::exception_ptr e) override;
    };

    class ParserErrorStrategy : public antlr4::DefaultErrorStrategy {
    public:
        void recover(antlr4::Parser *recognizer, std::exception_ptr e) override;
        antlr4::Token *recoverInline(antlr4::Parser *recognizer) override;
        void sync(antlr4::Parser *recognizer) override;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_PARSER_ERROR_LISTENER_H