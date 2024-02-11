#ifndef TEMPO_CONFIG_PROGRAM_CONFIG_H
#define TEMPO_CONFIG_PROGRAM_CONFIG_H

#include <filesystem>
#include <string>
#include <vector>

#include "config_types.h"
#include "config_result.h"

namespace tempo_config {

    struct ProgramConfigOptions {
        /**
         * The path indicating where the tool configuration is located in the environment config. If empty
 *       * then the tool config will be the same as the environment config.
         */
        std::vector<std::string> toolLocator;

        /** config map containing entries which override the program config. */
        ConfigMap overrideProgramConfigMap;

        /** config map containing entries which override the vendor config. */
        ConfigMap overrideVendorConfigMap;

        /** The path to the user config directory. If empty then the config is not merged. */
        std::filesystem::path userConfigDirectoryPath;

        /** The path to the user vendor config directory. If empty then the config is not merged. */
        std::filesystem::path userVendorConfigDirectoryPath;

        /** The path to the distribution config directory. If empty then the config is not merged. */
        std::filesystem::path distConfigDirectoryPath;

        /** The path to the distribution vendor config directory. If empty then the config is not merged. */
        std::filesystem::path distVendorConfigDirectoryPath;
    };

    /**
     *
     */
    class ProgramConfig {
    public:
        static tempo_utils::Result<std::shared_ptr<ProgramConfig>> load(const ProgramConfigOptions &options);

        ConfigMap getProgramConfig() const;
        ConfigMap getToolConfig() const;
        ConfigMap getVendorConfig() const;

    private:
        ConfigMap m_programConfig;
        ConfigMap m_toolConfig;
        ConfigMap m_vendorConfig;

        ProgramConfig(
            const ConfigMap &programConfig,
            const ConfigMap &toolConfig,
            const ConfigMap &vendorConfig);
    };
}

#endif // TEMPO_CONFIG_PROGRAM_CONFIG_H
