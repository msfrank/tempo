#include <absl/strings/match.h>

#include <tempo_command/command_result.h>
#include <tempo_command/command_tokenizer.h>

tempo_command::Token::Token()
    : m_type(TokenType::INVALID)
{
}

tempo_command::Token::Token(TokenType type, std::string_view value)
    : m_type(type),
      m_value(value)
{
    TU_ASSERT (m_type != TokenType::INVALID);
    TU_ASSERT (!m_value.empty());
}

tempo_command::Token::Token(const Token &other)
    : m_type(other.m_type),
      m_value(other.m_value)
{
}

tempo_command::Token::Token(Token &&other) noexcept
{
    m_type = other.m_type;
    other.m_type = TokenType::INVALID;
    m_value.swap(other.m_value);
}

tempo_command::Token&
tempo_command::Token::operator=(const Token &other)
{
    if (this != &other) {
        m_type = other.m_type;
        m_value = other.m_value;
    }
    return *this;
}

tempo_command::Token&
tempo_command::Token::operator=(Token &&other) noexcept
{
    if (this != &other) {
        m_type = other.m_type;
        other.m_type = TokenType::INVALID;
        m_value.swap(other.m_value);
    }
    return *this;
}

tempo_command::TokenType
tempo_command::Token::getType() const
{
    return m_type;
}

std::string
tempo_command::Token::getValue() const
{
    return m_value;
}

std::string_view
tempo_command::Token::valueView() const
{
    return m_value;
}

bool
tempo_command::Token::operator==(const Token &other) const
{
    return m_type == other.m_type && m_value == other.m_value;
}

tempo_utils::Result<tempo_command::TokenVector>
tempo_command::tokenize_argv(int argc, const char **argv)
{
    TokenVector tokens;

    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];

        if (arg == nullptr) {
            TU_LOG_WARN << "encountered null arg at index " << i;
            break;
        }

        std::string token(arg);
        if (absl::StartsWith(token, "--")) {
            if (token.size() == 2)
                tokens.emplace_back(TokenType::OPT_END, token);
            else
                tokens.emplace_back(TokenType::LONG_OPT, token);
        } else if (absl::StartsWith(token, "-")) {
            if (token.size() == 1) {
                tokens.emplace_back(TokenType::ARGUMENT, token);
            } else {
                for (auto iterator = token.cbegin() + 1; iterator != token.cend(); iterator++) {
                    std::string opt("-");
                    opt.push_back(*iterator);
                    tokens.emplace_back(TokenType::SHORT_OPT, opt);
                }
            }
        } else {
            tokens.emplace_back(TokenType::ARGUMENT, token);
        }
    }

    return tokens;
}
