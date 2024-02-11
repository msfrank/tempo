
#include <tempo_config/config_serde.h>
#include <tempo_config/parse_directories.h>
#include <tempo_utils/file_result.h>

/**
 *
 * @param distributionVendorConfigDirectory
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::parse_distribution_vendor_config_directory(
    const std::filesystem::path &distributionVendorConfigDirectory)
{
    if (!std::filesystem::exists(distributionVendorConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "missing distribution vendor config directory {}", distributionVendorConfigDirectory.string());
    if (!std::filesystem::is_directory(distributionVendorConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "invalid distribution vendor config directory {}; expected a directory",
            distributionVendorConfigDirectory.string());

    auto parseConfigDirectoryResult = read_config_tree_directory(
        distributionVendorConfigDirectory, ".config");
    if (parseConfigDirectoryResult.isStatus())
        return parseConfigDirectoryResult.getStatus();
    return parseConfigDirectoryResult.getResult();
}

/**
 *
 * @param distributionConfigDirectory
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::parse_distribution_config_directory(
    const std::filesystem::path &distributionConfigDirectory)
{
    if (!std::filesystem::exists(distributionConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "missing distribution vendor config directory {}", distributionConfigDirectory.string());
    if (!std::filesystem::is_directory(distributionConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "invalid distribution vendor config directory {}; expected a directory",
            distributionConfigDirectory.string());

    auto parseConfigDirectoryResult = read_config_tree_directory(
        distributionConfigDirectory, ".config");
    if (parseConfigDirectoryResult.isStatus())
        return parseConfigDirectoryResult.getStatus();
    return parseConfigDirectoryResult.getResult();
}

/**
 *
 * @param userVendorConfigDirectory
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::parse_user_vendor_config_directory(
    const std::filesystem::path &userVendorConfigDirectory)
{
    if (!std::filesystem::exists(userVendorConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "missing user vendor config directory {}", userVendorConfigDirectory.string());
    if (!std::filesystem::is_directory(userVendorConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "invalid user vendor config directory {}; expected a directory",
            userVendorConfigDirectory.string());

    auto parseConfigDirectoryResult = read_config_tree_directory(
        userVendorConfigDirectory, ".config");
    if (parseConfigDirectoryResult.isStatus())
        return parseConfigDirectoryResult.getStatus();
    return parseConfigDirectoryResult.getResult();
}

/**
 *
 * @param userConfigDirectory
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::parse_user_config_directory(
    const std::filesystem::path &userConfigDirectory)
{
    if (!std::filesystem::exists(userConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "missing user config directory {}", userConfigDirectory.string());
    if (!std::filesystem::is_directory(userConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "invalid user config directory {}; expected a directory",
            userConfigDirectory.string());

    auto parseConfigDirectoryResult = read_config_tree_directory(
        userConfigDirectory, ".config");
    if (parseConfigDirectoryResult.isStatus())
        return parseConfigDirectoryResult.getStatus();
    return parseConfigDirectoryResult.getResult();
}

/**
 *
 * @param workspaceVendorConfigDirectory
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::parse_workspace_vendor_config_directory(
    const std::filesystem::path &workspaceVendorConfigDirectory)
{
    if (!std::filesystem::exists(workspaceVendorConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "missing workspace vendor config directory {}", workspaceVendorConfigDirectory.string());
    if (!std::filesystem::is_directory(workspaceVendorConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "invalid workspace vendor config directory {}; expected a directory",
            workspaceVendorConfigDirectory.string());

    auto parseConfigDirectoryResult = read_config_tree_directory(
        workspaceVendorConfigDirectory, ".config");
    if (parseConfigDirectoryResult.isStatus())
        return parseConfigDirectoryResult.getStatus();
    return parseConfigDirectoryResult.getResult();
}

/**
 *
 * @param workspaceConfigDirectory
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::parse_workspace_config_directory(
    const std::filesystem::path &workspaceConfigDirectory)
{
    if (!std::filesystem::exists(workspaceConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "missing workspace config directory {}", workspaceConfigDirectory.string());
    if (!std::filesystem::is_directory(workspaceConfigDirectory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "invalid workspace config directory {}; expected a directory",
            workspaceConfigDirectory.string());

    auto parseConfigDirectoryResult = read_config_tree_directory(
        workspaceConfigDirectory, ".config");
    if (parseConfigDirectoryResult.isStatus())
        return parseConfigDirectoryResult.getStatus();
    return parseConfigDirectoryResult.getResult();
}
