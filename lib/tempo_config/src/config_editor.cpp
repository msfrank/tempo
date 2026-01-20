
#include <ANTLRInputStream.h>

#include <tempo_config/config_editor.h>
#include <tempo_config/config_result.h>
#include <tempo_config/internal/json_piece.h>
#include <tempo_config/internal/piece_store.h>

#include "ConfigLexer.h"

tempo_config::ConfigEditor::ConfigEditor()
    : m_store(std::make_unique<internal::PieceStore>())
{
}

tempo_config::ConfigEditor::~ConfigEditor()
{
    reset();
}

tempo_utils::Status
tempo_config::ConfigEditor::parse(std::string_view utf8)
{
    return m_store->parse(utf8);
}

void
tempo_config::ConfigEditor::reset()
{
    m_store->reset();
}

inline int recalculate_index(
    int index,
    const tempo_config::internal::ArrayPiece *arrayPtr,
    bool &after)
{
    auto size = arrayPtr->elements.size();
    if (index < 0) {
        after = true;
        index = size + index;
    } else {
        after = false;
    }
    if (index < 0 || size <= index)
        return -1;
    return index;
}

tempo_utils::Status
tempo_config::ConfigEditor::insertNode(const ConfigPath &path, const ConfigNode &node)
{
    ConfigPathPart lastPart;
    auto *parentPtr = m_store->findParent(path, lastPart);
    if (parentPtr == nullptr)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "failed to insert node; node not found at path");

    switch (parentPtr->type) {
    case internal::PieceType::Array:
        {
            if (lastPart.getType() != ConfigPathPartType::Index)
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "failed to insert node; node not found at path");
            auto *arrayPtr = static_cast<internal::ArrayPiece *>(parentPtr);
            bool after;
            auto index = recalculate_index(lastPart.getIndex(), arrayPtr, after);
            internal::ElementPiece *insertedPtr;
            TU_ASSIGN_OR_RETURN (insertedPtr, arrayPtr->insert(index, after));
            return m_store->insert(node, insertedPtr);
        }
    case internal::PieceType::Object:
        {
            if (lastPart.getType() != ConfigPathPartType::Key)
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "failed to insert node; node not found at path");
            auto *objectPtr = static_cast<internal::ObjectPiece *>(parentPtr);
            internal::MemberPiece *insertedPtr;
            TU_ASSIGN_OR_RETURN (insertedPtr, objectPtr->insert(lastPart.getKey()));
            return m_store->insert(node, insertedPtr);
        }
    default:
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "failed to remove node; invalid path");
    }
}

tempo_utils::Status
tempo_config::ConfigEditor::removeNode(const ConfigPath &path)
{
    ConfigPathPart lastPart;
    auto *parentPtr = m_store->findParent(path, lastPart);
    if (parentPtr == nullptr)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "failed to remove node; node not found at path");

    switch (parentPtr->type) {
        case internal::PieceType::Array:
            {
                if (lastPart.getType() != ConfigPathPartType::Index)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "failed to remove node; node not found at path");
                auto *arrayPtr = static_cast<internal::ArrayPiece *>(parentPtr);
                return arrayPtr->remove(lastPart.getIndex());
            }
        case internal::PieceType::Object:
            {
                if (lastPart.getType() != ConfigPathPartType::Key)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "failed to remove node; node not found at path");
                auto *objectPtr = static_cast<internal::ObjectPiece *>(parentPtr);
                return objectPtr->remove(lastPart.getKey());
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "failed to remove node; invalid path");
    }
}

tempo_utils::Status
tempo_config::ConfigEditor::replaceNode(const ConfigPath &path, const ConfigNode &node)
{
    TU_RETURN_IF_NOT_OK (removeNode(path));
    return insertNode(path, node);
}

tempo_utils::Status
tempo_config::ConfigEditor::writeJson(std::string &out) const
{
    return m_store->writeJson(out);
}

std::string
tempo_config::ConfigEditor::toString() const
{
    std::string out;
    m_store->writeJson(out);
    return out;
}