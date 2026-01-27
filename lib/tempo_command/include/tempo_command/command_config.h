#ifndef TEMPO_COMMAND_COMMAND_CONFIG_H
#define TEMPO_COMMAND_COMMAND_CONFIG_H

#include <vector>

#include <absl/container/flat_hash_map.h>

#include <tempo_config/abstract_converter.h>
#include <tempo_config/config_types.h>

#include "command_result.h"
#include "command_parser.h"

namespace tempo_command {

    enum class MappingType {
        INVALID,
        ZERO_OR_ONE_INSTANCE,
        ONE_INSTANCE,
        ONE_OR_MORE_INSTANCES,
        ANY_INSTANCES,
        COUNT_INSTANCES,
        TRUE_IF_INSTANCE,
        FALSE_IF_INSTANCE,
        INSTANCE_HASH,
    };

    struct Mapping {
        std::string id;
        MappingType type;
        std::string meta;
    };

    typedef std::vector<Mapping> MappingVector;
    typedef absl::flat_hash_map<std::string, tempo_config::ConfigNode> CommandConfig;

    tempo_utils::Status convert_options(
        const OptionsHash &options,
        const MappingVector &mappings,
        const GroupingVector &groupings,
        CommandConfig &config);

    tempo_utils::Status convert_arguments(
        const ArgumentVector &arguments,
        const MappingVector &mappings,
        CommandConfig &config);

    template<class T>
    tempo_utils::Status parse_command_config(
        T &dst,
        const tempo_config::AbstractConverter<T> &parser,
        const CommandConfig &config,
        std::string_view key)
    {
        if (!config.contains(key)) {
            return parser.convertValue(tempo_config::ConfigNode(), dst);
        } else {
            return parser.convertValue(config.at(key), dst);
        }
    }

    std::string command_config_to_string(const CommandConfig &config);
}

#endif // TEMPO_COMMAND_COMMAND_CONFIG_H