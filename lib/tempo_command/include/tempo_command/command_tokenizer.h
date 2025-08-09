#ifndef TEMPO_COMMAND_COMMAND_TOKENIZER_H
#define TEMPO_COMMAND_COMMAND_TOKENIZER_H

#include <string>

#include <absl/hash/hash.h>

#include <tempo_utils/result.h>

namespace tempo_command {

    enum class TokenType {
        INVALID,
        SHORT_OPT,
        LONG_OPT,
        ARGUMENT,
        OPT_END,
    };

    class Token {
    public:
        Token();
        Token(TokenType type, std::string_view value);
        Token(const Token &other);
        Token(Token &&other) noexcept;

        Token& operator=(const Token &other);
        Token& operator=(Token &&other) noexcept;

        TokenType getType() const;
        std::string getValue() const;

        std::string_view valueView() const;

        bool operator==(const Token &other) const;

        template<typename H>
        friend H AbslHashValue(H h, const Token &token)
        {
            return H::combine(std::move(h), token.m_value, token.m_type);
        }

    private:
        TokenType m_type;
        std::string m_value;
    };

    typedef std::vector<Token> TokenVector;

    tempo_utils::Result<TokenVector> tokenize_argv(int argc, const char **argv);
}

#endif // TEMPO_COMMAND_COMMAND_TOKENIZER_H