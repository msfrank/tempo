
#include <absl/strings/substitute.h>

#include <tempo_command/command_config.h>

/**
 *
 * @param options
 * @param mappings
 * @param config
 * @return
 */
tempo_command::CommandStatus
tempo_command::convert_options(
    const OptionsHash &options,
    const MappingVector &mappings,
    CommandConfig &config)
{
    for (const auto &mapping : mappings) {
        switch (mapping.type) {

            case MappingType::ZERO_OR_ONE_INSTANCE: {
                if (options.contains(mapping.id)) {
                    const auto &values = options.at(mapping.id);
                    if (values.size() > 1)
                        return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                            "option {} must be specified once but found {} instances", mapping.id, values.size());
                    tempo_config::ConfigValue v(values.front());
                    config[mapping.id] = v;
                }
                break;
            }

            case MappingType::ONE_INSTANCE: {
                if (!options.contains(mapping.id))
                    return CommandStatus::forCondition(CommandCondition::kMissingCommandOption,
                        "value required for option {}", mapping.id);
                const auto &values = options.at(mapping.id);
                if (values.empty())
                    return CommandStatus::forCondition(CommandCondition::kMissingCommandOption,
                        "value required for option {}", mapping.id);
                if (values.size() > 1)
                    return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                        "option {} must be specified exactly once but found {} instances", mapping.id, values.size());
                tempo_config::ConfigValue v(values.front());
                config[mapping.id] = v;
                break;
            }

            case MappingType::ONE_OR_MORE_INSTANCES: {
                if (!options.contains(mapping.id))
                    return CommandStatus::forCondition(CommandCondition::kMissingCommandOption,
                        "value required for option {}", mapping.id);
                const auto &values = options.at(mapping.id);
                if (values.empty())
                    return CommandStatus::forCondition(CommandCondition::kMissingCommandOption,
                        "value required for option {}", mapping.id);
                std::vector<tempo_config::ConfigNode> seq;
                for (const auto &value : values) {
                    seq.push_back(tempo_config::ConfigValue(value));
                }
                config[mapping.id] = tempo_config::ConfigSeq(seq);
                break;
            }

            case MappingType::ANY_INSTANCES: {
                if (options.contains(mapping.id)) {
                    const auto &values = options.at(mapping.id);
                    std::vector<tempo_config::ConfigNode> seq;
                    for (const auto &value: values) {
                        seq.push_back(tempo_config::ConfigValue(value));
                    }
                    config[mapping.id] = tempo_config::ConfigSeq(seq);
                }
                break;
            }

            case MappingType::COUNT_INSTANCES: {
                if (options.contains(mapping.id)) {
                    const auto &values = options.at(mapping.id);
                    config[mapping.id] = tempo_config::ConfigValue(absl::StrCat(values.size()));
                } else {
                    config[mapping.id] = tempo_config::ConfigValue("0");
                }
                break;
            }

            case MappingType::TRUE_IF_INSTANCE: {
                if (options.contains(mapping.id)) {
                    const auto &values = options.at(mapping.id);
                    if (values.size() > 1)
                        return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                            "option {} must be specified exactly once but found {} instances", mapping.id, values.size());
                    config[mapping.id] = tempo_config::ConfigValue(!values.empty()? "true" : "false");
                } else {
                    config[mapping.id] = tempo_config::ConfigValue("false");
                }
                break;
            }

            case MappingType::FALSE_IF_INSTANCE: {
                if (options.contains(mapping.id)) {
                    const auto &values = options.at(mapping.id);
                    if (values.size() > 1)
                        return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                            "option {} must be specified exactly once but found {} instances", mapping.id, values.size());
                    config[mapping.id] = tempo_config::ConfigValue(values.empty()? "true" : "false");
                } else {
                    config[mapping.id] = tempo_config::ConfigValue("true");
                }
                break;
            }

            case MappingType::INSTANCE_HASH: {
                if (options.contains(mapping.id)) {
                    const auto &values = options.at(mapping.id);
                    if (values.size() % 2 != 0)
                        return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                            "missing hash value for option {}", mapping.id);
                    absl::flat_hash_map<std::string,tempo_config::ConfigNode> map;
                    for (int i = 0; i < static_cast<int>(values.size()); i += 2) {
                        const auto &k = values[i];
                        tempo_config::ConfigValue v(values[i + 1]);
                        map[k] = v;
                    }
                    config[mapping.id] = tempo_config::ConfigMap(map);
                }
                break;
            }

            case MappingType::INVALID:
            default:
                return CommandStatus::forCondition(
                    CommandCondition::kCommandInvariant, "invalid option mapping {}", mapping.id);
        }
    }

    return {};
}

/**
 *
 * @param arguments
 * @param mappings
 * @param config
 * @return
 */
tempo_command::CommandStatus
tempo_command::convert_arguments(
    const ArgumentVector &arguments,
    const MappingVector &mappings,
    CommandConfig &config)
{
    auto curr = arguments.cbegin();
    auto end = arguments.cend();

    for (const auto &mapping : mappings) {
        switch (mapping.type) {

            // if at least one argument is present, then consume it
            case MappingType::ZERO_OR_ONE_INSTANCE: {
                if (curr != end) {
                    config[mapping.id] = tempo_config::ConfigValue(*curr++);
                }
                break;
            }

            case MappingType::ONE_INSTANCE: {
                if (curr == end)
                    return CommandStatus::forCondition(CommandCondition::kMissingCommandOption,
                        "missing required argument {}", mapping.id);
                config[mapping.id] = tempo_config::ConfigValue(*curr++);
                break;
            }

            case MappingType::ONE_OR_MORE_INSTANCES: {
                if (curr == end)
                    return CommandStatus::forCondition(CommandCondition::kMissingCommandOption,
                        "missing required argument {}", mapping.id);
                std::vector<tempo_config::ConfigNode> seq;
                for (; curr != end; curr++) {
                    seq.push_back(tempo_config::ConfigValue(*curr));
                }
                config[mapping.id] = tempo_config::ConfigSeq(seq);
                break;
            }

            case MappingType::ANY_INSTANCES: {
                std::vector<tempo_config::ConfigNode> seq;
                for (; curr != end; curr++) {
                    seq.push_back(tempo_config::ConfigValue(*curr));
                }
                config[mapping.id] = tempo_config::ConfigSeq(seq);
                break;
            }

            case MappingType::COUNT_INSTANCES: {
                int count = 0;
                for (; curr != end; curr++) {
                    count++;
                }
                config[mapping.id] = tempo_config::ConfigValue(absl::StrCat(count));
                break;
            }

            // if instance is present, then consume it and set mapping to true
            case MappingType::TRUE_IF_INSTANCE: {
                bool v = false;
                if (curr != end) {
                    v = true;
                    curr++;
                }
                config[mapping.id] = tempo_config::ConfigValue(v? "true" : "false");
                break;
            }

            case MappingType::FALSE_IF_INSTANCE: {
                bool v = true;
                if (curr != end) {
                    v = false;
                    curr++;
                }
                config[mapping.id] = tempo_config::ConfigValue(v? "true" : "false");
                break;
            }

            case MappingType::INVALID:
            default:
                return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
                    "invalid argument mapping {}", mapping.id);
        }
    }

    if (curr != end)
        return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
            "unexpected argument '{}'", *curr);

    return {};
}

/**
 *
 * @param config
 * @return
 */
std::string
tempo_command::command_config_to_string(const CommandConfig &config)
{
    std::string s;
    s.push_back('{');
    auto iterator = config.cbegin();
    if (iterator != config.cend()) {
        s.push_back('"');
        s.append(iterator->first);
        s.push_back('"');
        s.push_back(':');
        s.append(iterator->second.toString());
        for (++iterator; iterator != config.cend(); iterator++) {
            s.push_back(',');
            s.push_back('"');
            s.append(iterator->first);
            s.push_back('"');
            s.push_back(':');
            s.append(iterator->second.toString());
        }
    }
    s.push_back('}');
    return s;
}
