#ifndef TEMPO_CONFIG_CONFIG_SERDE_H
#define TEMPO_CONFIG_CONFIG_SERDE_H

#include <filesystem>

#include <tempo_utils/result.h>

#include "config_types.h"

namespace tempo_config {

    tempo_utils::Result<ConfigNode> read_config_string(std::string_view utf8, std::shared_ptr<ConfigSource> source = {});

    tempo_utils::Result<ConfigNode> read_config_file(const std::filesystem::path &path);

    tempo_utils::Result<ConfigMap> read_config_map_file(const std::filesystem::path &path);

    tempo_utils::Result<ConfigMap> read_config_map_directory(
        const std::filesystem::path &directory,
        std::string_view extension = {});

    tempo_utils::Result<ConfigMap> read_config_tree_directory(
        const std::filesystem::path &directory,
        std::string_view extension = {});

    tempo_utils::Status write_config_string(const ConfigNode &root, std::string &bytes);

    tempo_utils::Status write_config_file(const ConfigNode &root, const std::filesystem::path &path);
}

#endif // TEMPO_CONFIG_CONFIG_SERDE_H
