#ifndef TEMPO_CONFIG_WORKSPACE_CONFIG_H
#define TEMPO_CONFIG_WORKSPACE_CONFIG_H

#include <filesystem>
#include <string>
#include <vector>

#include <tempo_config/config_types.h>
#include <tempo_config/config_result.h>

namespace tempo_config {

    struct WorkspaceConfigOptions {
        /**
         * The path indicating where the tool configuration is located in the workspace config. If empty
 *       * then the tool config will be the same as the workspace config.
         */
        std::vector<std::string> toolLocator;

        /** config map containing entries which override the workspace config. */
        ConfigMap overrideWorkspaceConfigMap;

        /** config map containing entries which override the vendor config. */
        ConfigMap overrideVendorConfigMap;

        std::string workspaceVendorConfigDirectoryNameOverride;

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
    class WorkspaceConfig {
    public:
        static tempo_utils::Result<std::shared_ptr<WorkspaceConfig>> load(
            const std::filesystem::path &workspaceConfigFilePath,
            const WorkspaceConfigOptions &options);
        static tempo_utils::Result<std::shared_ptr<WorkspaceConfig>> find(
            std::string_view workspaceConfigFileName,
            const std::filesystem::path &searchPathStart,
            const WorkspaceConfigOptions &options);

        std::filesystem::path getWorkspaceRoot() const;

        ConfigMap getWorkspaceConfig() const;
        ConfigMap getToolConfig() const;
        ConfigMap getVendorConfig() const;

    private:
        std::filesystem::path m_workspaceRoot;
        ConfigMap m_workspaceConfig;
        ConfigMap m_toolConfig;
        ConfigMap m_vendorConfig;

        WorkspaceConfig(
            const std::filesystem::path &workspaceRoot,
            const ConfigMap &workspaceConfig,
            const ConfigMap &toolConfig,
            const ConfigMap &vendorConfig);
    };
}

#endif // TEMPO_CONFIG_WORKSPACE_CONFIG_H