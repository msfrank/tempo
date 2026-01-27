#include <iostream>

#include <absl/strings/str_join.h>

#include <tempo_command/command_help.h>

#define COLUMN_MARGIN   4

void
tempo_command::display_help_and_exit(
    const std::vector<std::string> &commandPath,
    const std::string &commandDescription,
    const std::vector<Subcommand> &subcommands,
    const std::vector<Grouping> &commandGroupings,
    const std::vector<Mapping> &optMappings,
    const std::vector<Mapping> &argMappings,
    const std::vector<Default> &configDefaults)
{

    // construct option indexes keyed on id
    absl::flat_hash_map<std::string,Mapping> optMap;
    for (const auto &mapping: optMappings) {
        optMap[mapping.id] = mapping;
    }
    absl::flat_hash_map<std::string,Grouping> optGrp;
    for (const auto &grouping: commandGroupings) {
        optGrp[grouping.id] = grouping;
    }
    absl::flat_hash_map<std::string,Default> optDfl;
    for (const auto &dfl: configDefaults) {
        optDfl[dfl.id] = dfl;
    }

    // construct argument indexes keyed on id
    absl::flat_hash_map<std::string,Mapping> argMap;
    for (const auto &mapping: argMappings) {
        argMap[mapping.id] = mapping;
    }
    absl::flat_hash_map<std::string,Grouping> argGrp;
    for (const auto &grouping: commandGroupings) {
        argGrp[grouping.id] = grouping;
    }
    absl::flat_hash_map<std::string,Default> argDfl;
    for (const auto &dfl: configDefaults) {
        argDfl[dfl.id] = dfl;
    }

    std::vector<std::pair<std::string,std::string>> cmdLines;
    std::vector<std::pair<std::string,std::string>> optLines;
    std::vector<std::pair<std::string,std::string>> argLines;
    int firstColumnWidth = 0;

    // construct vector of command description rows
    for (const auto &subcommand : subcommands) {
        std::pair<std::string,std::string> line;
        line.first = subcommand.name;
        line.second = subcommand.description;
        cmdLines.push_back(line);
        firstColumnWidth = std::max(firstColumnWidth, static_cast<int>(line.first.size()));
    }

    // construct vector of option description rows
    for (const auto &map : optMappings) {
        const auto &grp = optGrp[map.id];
        const auto &dfl = optDfl[map.id];
        std::pair<std::string,std::string> line;
        line.first = absl::StrJoin(grp.matches, ",") + (dfl.meta.empty()? "" : " " + dfl.meta);
        line.second = dfl.description.empty()? "" : dfl.description;
        optLines.push_back(line);
        firstColumnWidth = std::max(firstColumnWidth, static_cast<int>(line.first.size()));
    }

    // construct vector of argument description rows
    for (const auto &map : argMappings) {
        const auto &dfl = argDfl[map.id];
        std::pair<std::string,std::string> line;
        line.first = dfl.meta;
        line.second = dfl.description.empty()? "" : dfl.description;
        argLines.push_back(line);
        firstColumnWidth = std::max(firstColumnWidth, static_cast<int>(line.first.size()));
    }

    // round up the first column width to a multiple of the COLUMN_MARGIN
    firstColumnWidth = firstColumnWidth + (firstColumnWidth % COLUMN_MARGIN) + COLUMN_MARGIN;

    // print the usage line without newline separator
    std::cout << "Usage: " << absl::StrJoin(commandPath, " ");

    // print each option on the usage line if any are specified
    for (const auto &map : optMappings) {
        const auto &grp = optGrp[map.id];
        const auto &dfl = optDfl[map.id];

        auto opt = grp.matches.front();
        switch (grp.type) {
            case GroupingType::NO_ARGUMENT:
            case GroupingType::HELP_FLAG:
            case GroupingType::VERSION_FLAG:
                std::cout << " " << opt;
                break;
            case GroupingType::SINGLE_ARGUMENT:
                std::cout << " " << opt << " " << dfl.meta;
                break;
            case GroupingType::MULTI_ARGUMENT:
                std::cout << " " << opt << " " << dfl.meta << "...";
                break;
            case GroupingType::KV_ARGUMENT:
                std::cout << " " << opt << " [KEY VALUE]";
                break;
            case GroupingType::INVALID:
                break;
        }
    }

    // print subcommands on the usage line if any are specified
    if (!subcommands.empty())
        std::cout << " COMMAND";

    // print each argument on the usage line if any are specified
    for (const auto &map : argMappings) {
        const auto &dfl = argDfl[map.id];
        switch (map.type) {
            case MappingType::ONE_INSTANCE:
            case MappingType::INSTANCE_HASH:
                std::cout << " " << dfl.meta;
                break;
            case MappingType::ZERO_OR_ONE_INSTANCE:
            case MappingType::TRUE_IF_INSTANCE:
            case MappingType::FALSE_IF_INSTANCE:
                std::cout << " [" << dfl.meta << "]";
                break;
            case MappingType::ONE_OR_MORE_INSTANCES:
                std::cout << " " << dfl.meta << "...";
                break;
            case MappingType::ANY_INSTANCES:
            case MappingType::COUNT_INSTANCES:
                std::cout << " [" << dfl.meta << "...]";
                break;
            case MappingType::INVALID:
                break;
        }
    }

    // print newline separator to end the usage line
    std::cout << std::endl;

    // print the description line if it is specified
    if (!commandDescription.empty()) {
        std::cout << std::endl;
        std::cout << commandDescription << std::endl;
    }

    // print the list of options if there are any
    if (!optMappings.empty()) {
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        for (const auto &line : optLines) {
            for (int i = 0; i < COLUMN_MARGIN; i++) { std::cout.put(' '); }
            std::cout << std::left << std::setw(firstColumnWidth)
                      << line.first
                      << line.second
                      << std::endl;
        }
    }

    // print the list of subcommands if there are any
    if (!subcommands.empty()) {
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        for (const auto &line : cmdLines) {
            for (int i = 0; i < COLUMN_MARGIN; i++) { std::cout.put(' '); }
            std::cout << std::left << std::setw(firstColumnWidth)
                      << line.first
                      << line.second
                      << std::endl;
        }
    }

    // print the list of arguments if there are any
    if (!argMappings.empty()) {
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        for (const auto &line : argLines) {
            for (int i = 0; i < COLUMN_MARGIN; i++) { std::cout.put(' '); }
            std::cout << std::left << std::setw(firstColumnWidth)
                      << line.first
                      << line.second
                      << std::endl;
        }
    }

    std::exit(0);
}

void
tempo_command::display_version_and_exit(const char *versionString)
{
    TU_ASSERT (versionString != nullptr);
    std::cout << "version " << versionString << std::endl;
    exit(0);
}

void
tempo_command::display_status_and_exit(const tempo_utils::Status &status, int exitstatus)
{
    auto message = status.toString();
    std::cout << message << std::endl;
    exit(exitstatus);
}
