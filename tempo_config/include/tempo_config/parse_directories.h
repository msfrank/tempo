#ifndef TEMPO_CONFIG_VENDOR_CONFIG_H
#define TEMPO_CONFIG_VENDOR_CONFIG_H

#include <filesystem>

#include <tempo_utils/result.h>

#include "config_types.h"

namespace tempo_config {

    tempo_utils::Result<ConfigMap> parse_distribution_vendor_config_directory(
        const std::filesystem::path &distributionVendorConfigDirectory);

    tempo_utils::Result<ConfigMap> parse_distribution_config_directory(
        const std::filesystem::path &distributionConfigDirectory);

    tempo_utils::Result<ConfigMap> parse_user_vendor_config_directory(
        const std::filesystem::path &userVendorConfigDirectory);

    tempo_utils::Result<ConfigMap> parse_user_config_directory(
        const std::filesystem::path &userConfigDirectory);

    tempo_utils::Result<ConfigMap> parse_workspace_vendor_config_directory(
        const std::filesystem::path &workspaceVendorConfigDirectory);

    tempo_utils::Result<ConfigMap> parse_workspace_config_directory(
        const std::filesystem::path &workspaceConfigDirectory);
}

#endif // TEMPO_CONFIG_VENDOR_CONFIG_H
