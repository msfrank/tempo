
#include <filesystem>

#include <tempo_config/merge_map.h>
#include <tempo_config/parse_directories.h>
#include <tempo_config/program_config.h>

tempo_config::ProgramConfig::ProgramConfig(
    const ConfigMap &programConfig,
    const ConfigMap &toolConfig,
    const ConfigMap &vendorConfig)
    : m_programConfig(programConfig),
      m_toolConfig(toolConfig),
      m_vendorConfig(vendorConfig)
{
}

tempo_config::ConfigMap
tempo_config::ProgramConfig::getProgramConfig() const
{
    return m_programConfig;
}

tempo_config::ConfigMap
tempo_config::ProgramConfig::getToolConfig() const
{
    return m_toolConfig;
}

tempo_config::ConfigMap
tempo_config::ProgramConfig::getVendorConfig() const
{
    return m_vendorConfig;
}

/**
 * Construct a program. The tool config will be set to the config found at the specified tool path.
 * The vendor configs in the user root and distribution root will be merged into the workspace config.
 *
 * @param options The program options.
 * @return The workspace.
 */
tempo_utils::Result<std::shared_ptr<tempo_config::ProgramConfig>>
tempo_config::ProgramConfig::load(const ProgramConfigOptions &options)
{
    // parse user and dist configs in reverse priority order
    ConfigMap programConfig;

    // if distribution config root directory is specified then parse the configs
    if (exists(options.distConfigDirectoryPath)) {
        auto parseConfigDirectoryResult = parse_distribution_config_directory(
            options.distConfigDirectoryPath);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        programConfig = merge_map(programConfig, parseConfigDirectoryResult.getResult());
    }

    // if user config root directory is specified then parse the configs
    if (exists(options.userConfigDirectoryPath)) {
        auto parseConfigDirectoryResult = parse_user_config_directory(
            options.userConfigDirectoryPath);
        if (parseConfigDirectoryResult.isStatus())
            return parseConfigDirectoryResult.getStatus();
        programConfig = merge_map(programConfig, parseConfigDirectoryResult.getResult());
    }

    // if override program config is specified then merge it
    if (options.overrideProgramConfigMap.mapSize() > 0) {
        programConfig = merge_map(programConfig, options.overrideProgramConfigMap);
    }

    // get the program config at the tool path
    auto toolConfig = programConfig;
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

    // if override vendor config is specified then merge it
    if (options.overrideVendorConfigMap.mapSize() > 0) {
        vendorConfig = merge_map(vendorConfig, options.overrideVendorConfigMap);
    }

    return std::shared_ptr<ProgramConfig>(new ProgramConfig(
        programConfig,
        toolConfig,
        vendorConfig));
}