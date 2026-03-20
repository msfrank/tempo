#ifndef TEMPO_COMMAND_LOGGING_ENVIRONMENT_H
#define TEMPO_COMMAND_LOGGING_ENVIRONMENT_H

#include <filesystem>

#include <tempo_utils/status.h>

namespace tempo_command {

    constexpr const char * const kEnvOverrideLoggingConfigName = "TEMPO_OVERRIDE_LOGGING_CONFIG";

    constexpr const char * const kEnvLoggingConfigFileName = "TEMPO_LOGGING_CONFIG_FILE";

    tempo_utils::Status init_logging_from_environment(
        const std::filesystem::path &loggingConfigFile,
        const tempo_config::ConfigMap &loggingOverrides = {},
        bool displayShortForm = false,
        bool logToStdout = false);

}

#endif // TEMPO_COMMAND_LOGGING_ENVIRONMENT_H