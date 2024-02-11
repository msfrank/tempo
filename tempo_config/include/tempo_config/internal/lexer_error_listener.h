#ifndef TEMPO_CONFIG_INTERNAL_LEXER_ERROR_LISTENER_H
#define TEMPO_CONFIG_INTERNAL_LEXER_ERROR_LISTENER_H

#include <memory>

#include <antlr4-runtime.h>

namespace tempo_config::internal {

    class LexerErrorListener : public antlr4::BaseErrorListener {

    public:
        void syntaxError(
            antlr4::Recognizer *recognizer,
            antlr4::Token *offendingSymbol,
            size_t line,
            size_t charPositionInLine,
            const std::string &message,
            std::exception_ptr e) override;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_LEXER_ERROR_LISTENER_H