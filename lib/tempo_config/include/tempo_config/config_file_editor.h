#ifndef TEMPO_CONFIG_CONFIG_FILE_EDITOR_H
#define TEMPO_CONFIG_CONFIG_FILE_EDITOR_H

#include <filesystem>

#include <tempo_utils/status.h>

#include "config_editor.h"
#include "config_path.h"
#include "config_types.h"

namespace tempo_config {

    class ConfigFileEditor {
    public:
        explicit ConfigFileEditor(const std::filesystem::path &configFilePath);

        bool isValid() const;
        tempo_utils::Status getStatus() const;

        bool hasNode(const ConfigPath &path) const;
        tempo_utils::Status insertNode(const ConfigPath &path, const ConfigNode &node);
        tempo_utils::Status replaceNode(const ConfigPath &path, const ConfigNode &node);
        tempo_utils::Status removeNode(const ConfigPath &path);

        tempo_utils::Status writeFile(const PrinterOptions &options = {});

    private:
        std::filesystem::path m_configFilePath;
        tempo_utils::Status m_status;
        std::unique_ptr<ConfigEditor> m_editor;
    };
}

#endif // TEMPO_CONFIG_CONFIG_FILE_EDITOR_H