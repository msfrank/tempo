#ifndef TEMPO_CONFIG_CONFIG_EDITOR_H
#define TEMPO_CONFIG_CONFIG_EDITOR_H

#include <string>
#include <stack>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

#include "config_path.h"
#include "config_types.h"

namespace tempo_config {

    // forward declarations
    namespace internal {
        class PieceStore;
    }

    class ConfigEditor {
    public:
        ConfigEditor();
        ~ConfigEditor();

        tempo_utils::Status parse(std::string_view utf8);

        tempo_utils::Status insertNode(const ConfigPath &path, const ConfigNode &node);
        tempo_utils::Status replaceNode(const ConfigPath &path, const ConfigNode &node);
        tempo_utils::Status removeNode(const ConfigPath &path);

        void reset();

        tempo_utils::Status writeJson(std::string &out, const PrinterOptions &options = {}) const;

    private:
        std::unique_ptr<internal::PieceStore> m_store;
    };
}

#endif // TEMPO_CONFIG_CONFIG_EDITOR_H