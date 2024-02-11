#ifndef TEMPO_COMMAND_COMMAND_PARSER_H
#define TEMPO_COMMAND_COMMAND_PARSER_H

#include <filesystem>
#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include "command_tokenizer.h"

namespace tempo_command {

    typedef tempo_utils::Status (*CommandFunc)(const std::filesystem::path &, const std::filesystem::path &, TokenVector &);

    struct Command {
        std::string name;
        CommandFunc func;
        std::string description;
    };

    enum class GroupingType {
        INVALID,
        NO_ARGUMENT,
        SINGLE_ARGUMENT,
        KV_ARGUMENT,
        MULTI_ARGUMENT,
        HELP_FLAG,
        VERSION_FLAG,
    };

    struct Grouping {
        std::string id;
        std::vector<std::string> matches;
        GroupingType type;
    };

    enum class TerminatorType {
        INVALID,
        ARGUMENT_END,
        FIRST_POSITIONAL,
        MATCHING_TOKEN,
    };

    struct Terminator {
        TerminatorType type;
        Token match;
    };

    typedef std::vector<Grouping> GroupingVector;
    typedef absl::flat_hash_map<std::string,std::vector<std::string>> OptionsHash;
    typedef std::vector<std::string> ArgumentVector;

    tempo_utils::Status
    parse_completely(
        TokenVector &tokens,
        const GroupingVector &groupings,
        OptionsHash &options,
        ArgumentVector &arguments);

    tempo_utils::Status
    parse_until_subcommand(
        TokenVector &tokens,
        const std::vector<Command> &commands,
        const GroupingVector &groupings,
        std::string &subcommand,
        OptionsHash &options);

    tempo_utils::Status
    parse_until_matching_token(
        TokenVector &tokens,
        const GroupingVector &groupings,
        OptionsHash &options,
        ArgumentVector &arguments,
        const Token &match);
}

#endif // TEMPO_COMMAND_COMMAND_PARSER_H
