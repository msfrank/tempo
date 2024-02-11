#ifndef TEMPO_COMMAND_COMMAND_TOKENIZER_H
#define TEMPO_COMMAND_COMMAND_TOKENIZER_H

#include <string>

#include <absl/hash/hash.h>

#include "command_result.h"

namespace tempo_command {

    enum class TokenType {
        INVALID,
        SHORT_OPT,
        LONG_OPT,
        ARGUMENT,
        OPT_END,
    };

    struct Token {
        TokenType type;
        std::string value;
    };

    bool operator==(const Token &lhs, const Token &rhs);

    template<typename H>
    H AbslHashValue(H h, const Token &token) {
        return H::combine(std::move(h), token.type, token.value);
    }

    typedef std::vector<Token> TokenVector;

    CommandResult<TokenVector> tokenize_argv(int argc, const char **argv);
}

#endif // TEMPO_COMMAND_COMMAND_TOKENIZER_H