#ifndef TEMPO_COMMAND_COMMAND_H
#define TEMPO_COMMAND_COMMAND_H

#include "command_tokenizer.h"
#include "command_help.h"
#include "command_parser.h"

namespace tempo_command {

    class Command {
    public:
        explicit Command(const std::vector<std::string> &commandPath);
        Command(const std::vector<std::string> &commandPath, const std::vector<Subcommand> &subcommands);

        tempo_utils::Status addArgument(
            const std::string &id,
            const std::string &meta,
            MappingType mapping,
            std::string_view description = {});

        tempo_utils::Status addFlag(
            const std::string &id,
            const std::vector<std::string> &matches,
            MappingType mapping,
            std::string_view description = {});

        tempo_utils::Status addOption(
            const std::string &id,
            const std::vector<std::string> &matches,
            MappingType mapping,
            std::string_view description = {},
            std::string_view meta = {});

        tempo_utils::Status addMultipleValueOption(
            const std::string &id,
            const std::vector<std::string> &matches,
            MappingType mapping,
            std::string_view description = {},
            std::string_view meta = {});

        tempo_utils::Status addKeyValueOption(
            const std::string &id,
            const std::vector<std::string> &matches,
            MappingType mapping,
            std::string_view description = {},
            std::string_view meta = {});

        tempo_utils::Status addHelpOption(
            const std::string &id,
            const std::vector<std::string> &matches,
            const std::string &description);

        tempo_utils::Status addVersionOption(
            const std::string &id,
            const std::vector<std::string> &matches,
            const std::string &version);

        tempo_utils::Status parse(int argc, const char *argv[]);
        tempo_utils::Status parseCompletely(TokenVector &tokens);
        tempo_utils::Status parseUntilMatchingToken(TokenVector &tokens, const Token &match);
        tempo_utils::Status parseUntilSubcommand(TokenVector &tokens, int &selected);

        bool hasConfig(std::string_view id) const;
        tempo_config::ConfigNode getConfig(std::string_view id) const;
        absl::flat_hash_map<std::string,tempo_config::ConfigNode>::const_iterator configBegin() const;
        absl::flat_hash_map<std::string,tempo_config::ConfigNode>::const_iterator configEnd() const;
        int configSize() const;


    private:
        std::vector<std::string> m_commandPath;
        std::vector<Subcommand> m_subcommands;
        std::vector<Default> m_defaults;
        std::vector<Grouping> m_groupings;
        std::vector<Mapping> m_optMappings;
        std::vector<Mapping> m_argMappings;
        bool m_hasHelp;
        std::string m_description;
        bool m_hasVersion;
        std::string m_version;
        absl::flat_hash_map<std::string,tempo_config::ConfigNode> m_config;

        struct CommandEntry {
            int dflIndex;
            int grpIndex;
            int mapIndex;
            bool isOption;
        };
        absl::flat_hash_map<std::string, CommandEntry> m_entries;

    public:
        template<class T>
        tempo_utils::Status convert(
            T &dst,
            const tempo_config::AbstractConverter<T> &converter,
            std::string_view key)
        {
            auto commandEntry = m_entries.find(key);
            if (commandEntry == m_entries.cend())
                return CommandStatus::forCondition(CommandCondition::kCommandInvariant,
                    "invalid command entry '{}'", key);
            const auto &command = commandEntry->second;

            auto configEntry = m_config.find(key);
            tempo_utils::Status status;
            if (configEntry == m_config.cend()) {
                status = converter.convertValue(tempo_config::ConfigNode{}, dst);
            } else {
                status = converter.convertValue(configEntry->second, dst);
            }
            if (status.isOk())
                return {};
            if (command.isOption) {
                const auto &grp = m_groupings.at(command.grpIndex);
                const auto &firstMatch = grp.matches.front();
                return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                    "invalid option {}; {}", firstMatch, status.getMessage());
            }
            const auto &dfl = m_defaults.at(command.dflIndex);
            return CommandStatus::forCondition(CommandCondition::kInvalidConfiguration,
                "invalid argument {}; {}", dfl.meta, status.getMessage());
        }
    };
}

#endif // TEMPO_COMMAND_COMMAND_H