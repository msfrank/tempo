#include <absl/strings/match.h>

#include <tempo_command/command_parser.h>
#include <tempo_utils/log_stream.h>

static tempo_command::CommandStatus
parse_tokens(
    tempo_command::TokenVector &tokens,
    const tempo_command::GroupingVector &groupings,
    tempo_command::OptionsHash &options,
    tempo_command::ArgumentVector &arguments,
    const tempo_command::Terminator &terminator)
{
    absl::flat_hash_map<tempo_command::Token,int> groupingIndex;

    //
    for (int i = 0; i < static_cast<int>(groupings.size()); i++) {
        const auto &grouping = groupings[i];
        for (const auto &match : grouping.matches) {
            tempo_command::Token token = {tempo_command::TokenType::INVALID};
            if (absl::StartsWith(match, "--") && match.size() > 2)
                token = {tempo_command::TokenType::LONG_OPT, match};
            else if (absl::StartsWith(match, "-") && match.size() == 2)
                token = {tempo_command::TokenType::SHORT_OPT, match};
            TU_ASSERT (token.type != tempo_command::TokenType::INVALID);
            TU_ASSERT (!groupingIndex.contains(token));
            groupingIndex[token] = i;
        }
    }

    while (!tokens.empty()) {

        const auto &first = tokens.front();

        // perform special handling of OPT_END depending on the terminator type
        switch (first.type) {
            case tempo_command::TokenType::INVALID:
                return tempo_command::CommandStatus(tempo_command::CommandCondition::kInvalidToken);
            case tempo_command::TokenType::ARGUMENT:
            case tempo_command::TokenType::SHORT_OPT:
            case tempo_command::TokenType::LONG_OPT:
                break;
            case tempo_command::TokenType::OPT_END: {
                tokens.pop_back();  // discard the OPT_END token
                if (terminator.type == tempo_command::TerminatorType::FIRST_POSITIONAL)
                    return {};
                while (!tokens.empty()) {
                    const auto &value = tokens.front().value;
                    if (terminator.type == tempo_command::TerminatorType::MATCHING_TOKEN
                        && terminator.match.type == tempo_command::TokenType::ARGUMENT
                        && terminator.match.value == tokens.front().value)
                        return {};
                    arguments.push_back(value);
                    tokens.erase(tokens.cbegin());
                }
                return {};
            }
        }

        // if terminating on matching token and current token matches the terminator, then we are done
        if (terminator.type == tempo_command::TerminatorType::MATCHING_TOKEN
            && first == terminator.match)
            return {};

        // if terminating on first positional argument and token is an argument, then we are done
        if (terminator.type == tempo_command::TerminatorType::FIRST_POSITIONAL
            && first.type == tempo_command::TokenType::ARGUMENT)
            return {};

        // otherwise pop the next token and continue processing
        auto token = tokens.front();
        tokens.erase(tokens.cbegin());

        // if token is an argument then push it onto the arguments vector and continue to next token
        if (token.type == tempo_command::TokenType::ARGUMENT) {
            arguments.push_back(token.value);
            continue;
        }

        // get the grouping for the option token
        if (!groupingIndex.contains(token))
            return tempo_command::CommandStatus::forCondition(
                tempo_command::CommandCondition::kCommandInvariant, "unknown option {}", token.value);
        const auto &grouping = groupings[groupingIndex[token]];

        auto &optlist = options[grouping.id];

        switch (grouping.type) {

            // option expects no argument token, so append empty token to optlist
            case tempo_command::GroupingType::NO_ARGUMENT: {
                optlist.push_back({});
                break;
            }

            // option expects exactly one argument token
            case tempo_command::GroupingType::SINGLE_ARGUMENT: {
                if (tokens.empty())
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kCommandInvariant,
                        "missing required value for option {}", grouping.id);
                token = tokens.front();
                tokens.erase(tokens.cbegin());
                if (token.type != tempo_command::TokenType::ARGUMENT)
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kInvalidToken,
                        "missing required value for option {}", grouping.id);
                optlist.push_back(token.value);
                break;
            }

            // option expects two argument tokens, one for the key and one for the value
            case tempo_command::GroupingType::KV_ARGUMENT: {
                if (tokens.empty())
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kCommandInvariant,
                        "missing key for option {}", grouping.id);
                token = tokens.front();
                tokens.erase(tokens.cbegin());
                if (token.type != tempo_command::TokenType::ARGUMENT)
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kInvalidToken,
                        "missing key for option {}", grouping.id);
                optlist.push_back(token.value);
                if (tokens.empty())
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kInvalidToken,
                        "missing value for option {}", grouping.id);
                token = tokens.front();
                tokens.erase(tokens.cbegin());
                if (token.type != tempo_command::TokenType::ARGUMENT)
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kInvalidToken,
                        "missing value for option {}", grouping.id);
                optlist.push_back(token.value);
                break;
            }

            // option expects one or more argument tokens
            case tempo_command::GroupingType::MULTI_ARGUMENT: {
                if (tokens.empty())
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kCommandInvariant,
                        "missing value for option {}", grouping.id);
                token = tokens.front();
                tokens.erase(tokens.cbegin());
                if (token.type != tempo_command::TokenType::ARGUMENT)
                    return tempo_command::CommandStatus::forCondition(
                        tempo_command::CommandCondition::kInvalidToken,
                        "missing value for option {}", grouping.id);
                optlist.push_back(token.value);
                while (!tokens.empty()) {
                    if (tokens.front().type != tempo_command::TokenType::ARGUMENT)
                        break;
                    token = tokens.front();
                    tokens.erase(tokens.cbegin());
                    optlist.push_back(token.value);
                }
                break;
            }

            // when the help flag token is encountered we immediately return without
            // processing the remaining tokens
            case tempo_command::GroupingType::HELP_FLAG:
                return tempo_command::CommandStatus::forCondition(
                    tempo_command::CommandCondition::kHelpRequested);

            // when the version flag token is encountered we immediately return without
            // processing the remaining tokens
            case tempo_command::GroupingType::VERSION_FLAG:
                return tempo_command::CommandStatus::forCondition(
                    tempo_command::CommandCondition::kVersionRequested);

            case tempo_command::GroupingType::INVALID:
            default:
                return tempo_command::CommandStatus::forCondition(
                    tempo_command::CommandCondition::kCommandInvariant,
                    "invalid grouping for option {}", grouping.id);
        }
    }

    return {};
}

/**
 *
 * @param tokens
 * @param groupings
 * @param options
 * @param arguments
 * @return
 */
tempo_utils::Status
tempo_command::parse_completely(
    TokenVector &tokens,
    const GroupingVector &groupings,
    OptionsHash &options,
    ArgumentVector &arguments)
{
    return parse_tokens(tokens, groupings, options, arguments, {TerminatorType::ARGUMENT_END});
}

/**
 *
 * @param tokens
 * @param commands
 * @param groupings
 * @param subcommand
 * @param options
 * @return
 */
tempo_utils::Status
tempo_command::parse_until_subcommand(
    TokenVector &tokens,
    const std::vector<Command> &commands,
    const GroupingVector &groupings,
    std::string &subcommand,
    OptionsHash &options)
{
    ArgumentVector arguments;
    auto status = parse_tokens(tokens, groupings, options, arguments, {TerminatorType::FIRST_POSITIONAL});
    if (status.notOk())
        return status;
    if (tokens.empty())
        return CommandStatus::forCondition(CommandCondition::kUnexpectedTokenEnd);

    auto token = tokens.front();
    tokens.erase(tokens.cbegin());
    subcommand = token.value;
    return {};
}

/**
 *
 * @param tokens
 * @param groupings
 * @param options
 * @param arguments
 * @param match
 * @return
 */
tempo_utils::Status
tempo_command::parse_until_matching_token(
    TokenVector &tokens,
    const GroupingVector &groupings,
    OptionsHash &options,
    ArgumentVector &arguments,
    const Token &match)
{
    return parse_tokens(tokens, groupings, options, arguments, {TerminatorType::ARGUMENT_END, match});
}
