
#include <tempo_command/command.h>

tempo_command::Command::Command(const std::vector<std::string> &commandPath)
    : m_commandPath(commandPath),
      m_hasHelp(false),
      m_hasVersion(false)
{
    TU_ASSERT (!m_commandPath.empty());
}

tempo_command::Command::Command(
    const std::vector<std::string> &commandPath,
    const std::vector<Subcommand> &subcommands)
    : m_commandPath(commandPath),
      m_subcommands(subcommands),
      m_hasHelp(false),
      m_hasVersion(false)
{
}

tempo_utils::Status
tempo_command::Command::addArgument(
    const std::string &id,
    const std::string &meta,
    MappingType mapping,
    std::string_view description)
{
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "command entry '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = false;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string(description), meta);
    entry.grpIndex = -1;
    entry.mapIndex = m_optMappings.size();
    m_argMappings.emplace_back(mapping, id);

    m_entries[id] = entry;
    return {};
}

tempo_utils::Status
tempo_command::Command::addFlag(
    const std::string &id,
    const std::vector<std::string> &matches,
    MappingType mapping,
    std::string_view description)
{
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "option '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = true;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string(description), std::string{});
    entry.grpIndex = m_groupings.size();
    m_groupings.emplace_back(id, matches, GroupingType::NO_ARGUMENT);
    entry.mapIndex = m_optMappings.size();
    m_optMappings.emplace_back(mapping, id);

    m_entries[id] = entry;
    return {};
}

tempo_utils::Status
tempo_command::Command::addOption(
    const std::string &id,
    const std::vector<std::string> &matches,
    MappingType mapping,
    std::string_view description,
    std::string_view meta)
{
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "option '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = true;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string(description), std::string(meta));
    entry.grpIndex = m_groupings.size();
    m_groupings.emplace_back(id, matches, GroupingType::SINGLE_ARGUMENT);
    entry.mapIndex = m_optMappings.size();
    m_optMappings.emplace_back(mapping, id);

    m_entries[id] = entry;
    return {};
}

tempo_utils::Status
tempo_command::Command::addMultipleValueOption(
    const std::string &id,
    const std::vector<std::string> &matches,
    MappingType mapping,
    std::string_view description,
    std::string_view meta)
{
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "option '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = true;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string(description), std::string(meta));
    entry.grpIndex = m_groupings.size();
    m_groupings.emplace_back(id, matches, GroupingType::MULTI_ARGUMENT);
    entry.mapIndex = m_optMappings.size();
    m_optMappings.emplace_back(mapping, id);

    m_entries[id] = entry;
    return {};
}

tempo_utils::Status
tempo_command::Command::addKeyValueOption(
    const std::string &id,
    const std::vector<std::string> &matches,
    MappingType mapping,
    std::string_view description,
    std::string_view meta)
{
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "option '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = true;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string(description), std::string(meta));
    entry.grpIndex = m_groupings.size();
    m_groupings.emplace_back(id, matches, GroupingType::KV_ARGUMENT);
    entry.mapIndex = m_optMappings.size();
    m_optMappings.emplace_back(mapping, id);

    m_entries[id] = entry;
    return {};
}

tempo_utils::Status
tempo_command::Command::addHelpOption(
    const std::string &id,
    const std::vector<std::string> &matches,
    const std::string &description)
{
    if (m_hasHelp)
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "help option is already defined");
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "option '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = true;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string("Display help"), std::string{});
    entry.grpIndex = m_groupings.size();
    m_groupings.emplace_back(id, matches, GroupingType::HELP_FLAG);
    entry.mapIndex = -1;

    m_entries[id] = entry;
    m_description = description;
    m_hasHelp = true;
    return {};
}

tempo_utils::Status
tempo_command::Command::addVersionOption(
    const std::string &id,
    const std::vector<std::string> &matches,
    const std::string &version)
{
    if (m_hasVersion)
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "version option is already defined");
    if (m_entries.contains(id))
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "option '{}' is already defined", id);

    CommandEntry entry;
    entry.isOption = true;
    entry.dflIndex = m_defaults.size();
    m_defaults.emplace_back(id, std::string("Display version"), std::string{});
    entry.grpIndex = m_groupings.size();
    m_groupings.emplace_back(id, matches, GroupingType::VERSION_FLAG);
    entry.mapIndex = -1;

    m_entries[id] = entry;
    m_version = version;
    m_hasVersion = true;
    return {};
}

tempo_utils::Status
tempo_command::Command::parse(int argc, const char *argv[])
{
    TokenVector tokens;
    TU_ASSIGN_OR_RETURN (tokens, tokenize_argv(argc, argv));
    return parseCompletely(tokens);
}

tempo_utils::Status
tempo_command::Command::parseCompletely(TokenVector &tokens)
{
    m_config.clear();

    OptionsHash options;
    ArgumentVector arguments;

    auto status = parse_completely(tokens, m_groupings, options, arguments);
    if (status.notOk())
    {
        if (status.matchesCondition(CommandCondition::kHelpRequested))
            display_help_and_exit(m_commandPath, m_description, m_subcommands,
                m_groupings, m_optMappings, m_argMappings, m_defaults);
        if (status.matchesCondition(CommandCondition::kVersionRequested))
            display_version_and_exit(m_version.c_str());
        return status;
    }

    TU_RETURN_IF_NOT_OK (tempo_command::convert_options(options, m_optMappings, m_config));
    TU_RETURN_IF_NOT_OK (tempo_command::convert_arguments(arguments, m_argMappings, m_config));
    return {};
}

tempo_utils::Status
tempo_command::Command::parseUntilMatchingToken(TokenVector &tokens, const Token &match)
{
    m_config.clear();

    OptionsHash options;
    ArgumentVector arguments;

    auto status = parse_until_matching_token(tokens, m_groupings, options, arguments, match);
    if (status.notOk())
    {
        if (status.matchesCondition(CommandCondition::kHelpRequested))
            display_help_and_exit(m_commandPath, m_description, m_subcommands,
                m_groupings, m_optMappings, m_argMappings, m_defaults);
        if (status.matchesCondition(CommandCondition::kVersionRequested))
            display_version_and_exit(m_version.c_str());
        return status;
    }

    TU_RETURN_IF_NOT_OK (tempo_command::convert_options(options, m_optMappings, m_config));
    TU_RETURN_IF_NOT_OK (tempo_command::convert_arguments(arguments, m_argMappings, m_config));
    return {};
}

tempo_utils::Status
tempo_command::Command::parseUntilSubcommand(TokenVector &tokens, int &selected)
{
    if (m_subcommands.empty())
        return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
            "no subcommands specified");

    m_config.clear();

    OptionsHash options;

    auto status = parse_until_subcommand(tokens, m_subcommands, m_groupings, selected, options);
    if (status.notOk())
    {
        if (status.matchesCondition(CommandCondition::kHelpRequested))
            display_help_and_exit(m_commandPath, m_description, m_subcommands,
                m_groupings, m_optMappings, m_argMappings, m_defaults);
        if (status.matchesCondition(CommandCondition::kVersionRequested))
            display_version_and_exit(m_version.c_str());
        return status;
    }

    TU_RETURN_IF_NOT_OK (tempo_command::convert_options(options, m_optMappings, m_config));
    return {};
}

bool
tempo_command::Command::hasConfig(std::string_view id) const
{
    return m_config.contains(id);
}

tempo_config::ConfigNode
tempo_command::Command::getConfig(std::string_view id) const
{
    auto entry = m_config.find(id);
    if (entry != m_config.cend())
        return entry->second;
    return {};
}

absl::flat_hash_map<std::string,tempo_config::ConfigNode>::const_iterator
tempo_command::Command::configBegin() const
{
    return m_config.cbegin();
}

absl::flat_hash_map<std::string,tempo_config::ConfigNode>::const_iterator
tempo_command::Command::configEnd() const
{
    return m_config.cend();
}

int
tempo_command::Command::configSize() const
{
    return m_config.size();
}
