#ifndef TEMPO_COMMAND_COMMAND_HELP_H
#define TEMPO_COMMAND_COMMAND_HELP_H

#include <tempo_utils/status.h>

#include "command_config.h"

namespace tempo_command {

    struct Default {
        std::string id;
        Option<std::string> value;
        std::string description;
        std::string meta;
    };

    typedef std::vector<Default> DefaultsVector;

    CommandConfig command_config_from_defaults(const std::vector<Default> &configDefaults);

    void display_help_and_exit(
        const std::vector<std::string> &commandPath,
        const std::string &commandDescription,
        const std::vector<Subcommand> &subcommands,
        const std::vector<Grouping> &commandGroupings,
        const std::vector<Mapping> &optMappings,
        const std::vector<Mapping> &argMappings,
        const std::vector<Default> &configDefaults = {});

    void display_version_and_exit(const char *versionString);

    void display_status_and_exit(const tempo_utils::Status &status, int exitstatus = 1);
}

#endif // TEMPO_COMMAND_COMMAND_HELP_H
