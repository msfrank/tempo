#include <absl/strings/match.h>

#include <tempo_command/command_tokenizer.h>

bool
tempo_command::operator==(const Token &lhs, const Token &rhs)
{
    return lhs.value == rhs.value && lhs.type == rhs.type;
}

tempo_command::CommandResult<tempo_command::TokenVector>
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
                tokens.push_back({TokenType::OPT_END, token});
            else
                tokens.push_back({TokenType::LONG_OPT, token});
        } else if (absl::StartsWith(token, "-")) {
            if (token.size() == 1) {
                tokens.push_back({TokenType::ARGUMENT, token});
            } else {
                for (auto iterator = token.cbegin() + 1; iterator != token.cend(); iterator++) {
                    std::string opt("-");
                    opt.push_back(*iterator);
                    tokens.push_back({TokenType::SHORT_OPT, opt});
                }
            }
        } else {
            tokens.push_back({TokenType::ARGUMENT, token});
        }
    }

    return tokens;
}
