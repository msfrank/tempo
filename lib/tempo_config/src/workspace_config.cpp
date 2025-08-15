
#include <tempo_config/config_utils.h>
#include <tempo_config/merge_map.h>
#include <tempo_config/parse_directories.h>
#include <tempo_config/workspace_config.h>
#include <tempo_utils/file_reader.h>
#include <tempo_utils/file_result.h>

tempo_config::WorkspaceConfig::WorkspaceConfig(
    const std::filesystem::path &workspaceRoot,
    const ConfigMap &workspaceConfig,
    const ConfigMap &toolConfig,
    const ConfigMap &vendorConfig)
    : m_workspaceRoot(workspaceRoot),
      m_workspaceConfig(workspaceConfig),
      m_toolConfig(toolConfig),
      m_vendorConfig(vendorConfig)
{
}

std::filesystem::path
tempo_config::WorkspaceConfig::getWorkspaceRoot() const
{
    return m_workspaceRoot;
}

tempo_config::ConfigMap
tempo_config::WorkspaceConfig::getWorkspaceConfig() const
{
    return m_workspaceConfig;
}

tempo_config::ConfigMap
tempo_config::WorkspaceConfig::getToolConfig() const
{
    return m_toolConfig;
}

tempo_config::ConfigMap
tempo_config::WorkspaceConfig::getVendorConfig() const
{
    return m_vendorConfig;
}

/**
 * Construct a workspace based on the specified workspace.config file. The tool config will be set to the
 * config found at the specified tool path. The vendor configs in the user root and distribution root will
 * be merged into the workspace config.
 *
 * @param workspaceConfigFile The path to the workspace.config
 * @param toolPath The path indicating where the tool configuration is located in the workspace config. If empty
 *     then the tool config will be the same as the workspace config.
 * @param userHomeDirectory The path to the user's home directory. If empty then user config is not merged.
 * @param distributionRootDirectory The path to the distribution root. If empty then distribution config is not merged.
 * @return The workspace.
 */
tempo_utils::Result<std::shared_ptr<tempo_config::WorkspaceConfig>>
tempo_config::WorkspaceConfig::load(
    const std::filesystem::path &workspaceConfigFilePath,
    const WorkspaceConfigOptions &options)
{
    // config path must exist
    if (!std::filesystem::exists(workspaceConfigFilePath))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "workspace config {} not found", workspaceConfigFilePath.string());

    // config path must be regular file (or a symlink to a regular file)
    auto status = std::filesystem::status(workspaceConfigFilePath);
    switch (status.type()) {
        case std::filesystem::file_type::regular:
        case std::filesystem::file_type::symlink:
            break;
        default:
            return ConfigStatus::forCondition( ConfigCondition::kParseError,
                "invalid workspace config {}; expected a regular file or symlink", workspaceConfigFilePath.string());
    }

    auto workspaceRootDirectory = workspaceConfigFilePath.parent_path();

    // parse workspace, user, and dist configs in reverse priority order
    ConfigMap workspaceConfig;

    // if distribution config root directory is specified then parse the configs
    if (exists(options.distConfigDirectoryPath)) {
        auto parseConfigDirectoryResult = parse_distribution_config_directory(
            options.distConfigDirectoryPath);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        workspaceConfig = merge_map(workspaceConfig, parseConfigDirectoryResult.getResult());
    }

    // if user config root directory is specified then parse the configs
    if (exists(options.userConfigDirectoryPath)) {
        auto parseConfigDirectoryResult = parse_user_config_directory(
            options.userConfigDirectoryPath);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        workspaceConfig = merge_map(workspaceConfig, parseConfigDirectoryResult.getResult());
    }

    // parse the workspace config
    auto parseWorkspaceConfigResult = read_config_map_file(workspaceConfigFilePath);
    if (parseWorkspaceConfigResult.isStatus())
        return parseWorkspaceConfigResult.getStatus();
    workspaceConfig = merge_map(workspaceConfig, parseWorkspaceConfigResult.getResult());

    // if override workspace config is specified then merge it
    if (options.overrideWorkspaceConfigMap.mapSize() > 0) {
        workspaceConfig = merge_map(workspaceConfig, options.overrideWorkspaceConfigMap);
    }

    // get the config at the tool path
    auto toolConfig = workspaceConfig;
    for (const auto &part : options.toolLocator) {
        ConfigNode node;
        if (!toolConfig.mapGet(part, node))
            return ConfigStatus::forCondition(ConfigCondition::kMissingValue, "missing key '{}'", part);
        toolConfig = node.toMap();
    }

    // parse user and dist vendor configs in reverse priority order
    ConfigMap vendorConfig;

    // if distribution vendor config root directory is specified then parse the vendor configs
    if (exists(options.distVendorConfigDirectoryPath)) {
        auto parseConfigDirectoryResult = parse_distribution_vendor_config_directory(
            options.distVendorConfigDirectoryPath);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        vendorConfig = merge_map(vendorConfig, parseConfigDirectoryResult.getResult());
    }

    // if user vendor config root directory is specified then parse the vendor configs
    if (exists(options.userVendorConfigDirectoryPath)) {
        auto parseConfigDirectoryResult = parse_user_vendor_config_directory(
            options.userVendorConfigDirectoryPath);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        vendorConfig = merge_map(vendorConfig, parseConfigDirectoryResult.getResult());
    }

    // if workspace config root directory is specified then parse the vendor configs
    auto workspaceVendorConfigDirectoryName = !options.workspaceVendorConfigDirectoryNameOverride.empty() ?
        options.workspaceVendorConfigDirectoryNameOverride : std::string(kDefaultVendorConfigDirectoryName);
    auto workspaceVendorConfigDirectory = workspaceRootDirectory / workspaceVendorConfigDirectoryName;
    if (exists(workspaceVendorConfigDirectory)) {
        auto parseConfigDirectoryResult = parse_workspace_vendor_config_directory(
            workspaceVendorConfigDirectory);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        vendorConfig = merge_map(vendorConfig, parseConfigDirectoryResult.getResult());
    }

    // if override vendor config is specified then merge it
    if (options.overrideVendorConfigMap.mapSize() > 0) {
        vendorConfig = merge_map(vendorConfig, options.overrideVendorConfigMap);
    }

    return std::shared_ptr<WorkspaceConfig>(new WorkspaceConfig(
        workspaceRootDirectory,
        workspaceConfig,
        toolConfig,
        vendorConfig));
}

/**
 * Find a workspace config file by starting at `searchPathStart` and recursively searching
 * parent directories for a file named 'workspace.config' until either the file is found or
 * the root directory is reached. If the file is found then the absolute path to the file is
 * returned, otherwise FileNotFound status is returned. If `workspaceConfigFileNameOverride`
 * is specified then it will be used as the workspace config file name instead of the default.
 *
 * @param searchPathStart The file path to start the search from.
 * @param workspaceConfigFileNameOverride The file name to search for.
 * @return An absolute path to the file, otherwise `Status` if the search failed.
 */
tempo_utils::Result<std::filesystem::path>
tempo_config::find_workspace_config(
    const std::filesystem::path &searchPathStart,
    std::string_view workspaceConfigFileNameOverride)
{
    // the initial search path must exist and be a directory
    if (!std::filesystem::exists(searchPathStart))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "search path {} not found", searchPathStart.string());

    // if searchPathStart is not a directory, then cd to the parent directory
    auto path = searchPathStart;
    if (!std::filesystem::is_directory(path)) {
        path = path.parent_path();
    }

    std::string workspaceConfigFileName = workspaceConfigFileNameOverride.empty()?
        kDefaultWorkspaceConfigFileName : std::string(workspaceConfigFileNameOverride);

    // check each parent directory for a file called "workspace.config". if the file is found then we have
    // determined the workspace root. otherwise if no file is found then workspace detection is failed.
    std::filesystem::path workspaceConfigFilePath;
    while (path != path.root_path()) {
        auto file = path / workspaceConfigFileName;
        if (std::filesystem::exists(file)) {
            workspaceConfigFilePath = file;
            break;
        }
        path = path.parent_path();
    }

    // we were unable to find the workspace
    if (workspaceConfigFilePath.empty())
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "workspace config not found");

    return workspaceConfigFilePath;
}
