
#include <tempo_config/config_file_editor.h>

#include "tempo_utils/file_reader.h"
#include "tempo_utils/file_writer.h"

tempo_config::ConfigFileEditor::ConfigFileEditor(const std::filesystem::path &configFilePath)
    : m_configFilePath(configFilePath)
{
    TU_ASSERT (!m_configFilePath.empty());
    tempo_utils::FileReader reader(m_configFilePath);
    if (reader.isValid()) {
        m_editor = std::make_unique<ConfigEditor>();
        auto bytes = reader.getBytes();
        m_status = m_editor->parse(bytes->getStringView());
    } else {
        m_status = reader.getStatus();
    }
}

bool
tempo_config::ConfigFileEditor::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_config::ConfigFileEditor::getStatus() const
{
    return m_status;
}

bool
tempo_config::ConfigFileEditor::hasNode(const ConfigPath &path) const
{
    if (m_editor == nullptr)
        return false;
    return m_editor->hasNode(path);
}

tempo_utils::Status
tempo_config::ConfigFileEditor::insertNode(const ConfigPath &path, const ConfigNode &node)
{
    if (m_editor == nullptr)
        return m_status;
    return m_editor->insertNode(path, node);
}

tempo_utils::Status
tempo_config::ConfigFileEditor::replaceNode(const ConfigPath &path, const ConfigNode &node)
{
    if (m_editor == nullptr)
        return m_status;
    return m_editor->replaceNode(path, node);
}

tempo_utils::Status
tempo_config::ConfigFileEditor::removeNode(const ConfigPath &path)
{
    if (m_editor == nullptr)
        return m_status;
    return m_editor->removeNode(path);
}

tempo_utils::Status
tempo_config::ConfigFileEditor::writeFile(const PrinterOptions &options)
{
    if (m_editor == nullptr)
        return m_status;
    std::string output;
    TU_RETURN_IF_NOT_OK (m_editor->writeJson(output, options));
    tempo_utils::FileWriter writer(m_configFilePath, output, tempo_utils::FileWriterMode::OVERWRITE_ONLY);
    return writer.getStatus();
}