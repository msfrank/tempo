#ifndef TEMPO_COMMAND_COMMAND_PARSER_H
#define TEMPO_COMMAND_COMMAND_PARSER_H

#include <filesystem>
#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include "command_tokenizer.h"

namespace tempo_command {

    struct Subcommand {
        std::string name;
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
        GroupingType type;
        std::vector<std::string> matches;
    };

    typedef std::vector<Grouping> GroupingVector;
    typedef absl::flat_hash_map<std::string,std::vector<std::pair<std::string,std::string>>> OptionsHash;
    typedef std::vector<std::string> ArgumentVector;
    typedef std::vector<Subcommand> SubcommandVector;

    tempo_utils::Status
    parse_completely(
        TokenVector &tokens,
        const GroupingVector &groupings,
        OptionsHash &options,
        ArgumentVector &arguments);

    tempo_utils::Status
    parse_until_subcommand(
        TokenVector &tokens,
        const std::vector<Subcommand> &subcommands,
        const GroupingVector &groupings,
        int &selected,
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
