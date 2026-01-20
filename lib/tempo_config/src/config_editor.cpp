
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

static tempo_config::internal::JsonPiece *
find_parent_piece(
    const tempo_config::ConfigPath &path,
    tempo_config::internal::RootPiece *root,
    tempo_config::ConfigPathPart &last)
{
    if (path.isRoot())
        return nullptr;

    std::vector parts(path.partsBegin(), path.partsEnd());
    last = parts.back();
    parts.pop_back();

    tempo_config::internal::JsonPiece *currPtr = root->root;
    for (const auto &part : parts) {
        if (currPtr == nullptr)
            return nullptr;
        switch (part.getType()) {
        case tempo_config::ConfigPathPartType::Index:
            {
                if (currPtr->type != tempo_config::internal::PieceType::Array)
                    return nullptr;
                auto *arrayPtr = static_cast<tempo_config::internal::ArrayPiece *>(currPtr);
                auto index = part.getIndex();
                if (index < 0 || arrayPtr->elements.size() <= index)
                    return nullptr;
                auto *elementPtr = arrayPtr->elements.at(index);
                currPtr = elementPtr->element;
                break;
            }
        case tempo_config::ConfigPathPartType::Key:
            {
                if (currPtr->type != tempo_config::internal::PieceType::Object)
                    return nullptr;
                auto *objectPtr = static_cast<tempo_config::internal::ObjectPiece *>(currPtr);
                auto key = part.getKey();
                auto entry = objectPtr->keyIndex.find(key);
                if (entry == objectPtr->keyIndex.cend())
                    return nullptr;
                auto index = entry->second;
                if (index < 0 || objectPtr->members.size() <= index)
                    return nullptr;
                auto *memberPtr = objectPtr->members.at(index);
                currPtr = memberPtr->value;
                break;
            }
        default:
            return nullptr;
        }
    }

    return currPtr;
}

// /**
//  * Find the piece at the specified path. If the piece exists then return a pair containing the
//  * pointer to the piece and an optional pointer to the additional context piece.
//  *
//  * @param path
//  * @param root
//  * @return
//  */
// static std::pair<
//     tempo_config::internal::JsonPiece *,
//     tempo_config::internal::JsonPiece *>
// find_piece(const tempo_config::ConfigPath &path, tempo_config::internal::JsonPiece *root)
// {
//     tempo_config::internal::JsonPiece *currPtr = root;
//     tempo_config::internal::JsonPiece *parentPtr = nullptr;
//
//     for (auto it = path.partsBegin(); it != path.partsEnd(); ++it) {
//         if (currPtr == nullptr)
//             return {};
//         const auto &part = *it;
//         switch (part.getType()) {
//             case tempo_config::ConfigPathPartType::Index:
//                 {
//                     if (currPtr->type != tempo_config::internal::PieceType::Array)
//                         return {};
//                     auto *arrayPtr = static_cast<tempo_config::internal::ArrayPiece *>(currPtr);
//                     auto index = part.getIndex();
//                     if (index < 0 || arrayPtr->elements.size() <= index)
//                         return {};
//                     parentPtr = currPtr;
//                     auto *elementPtr = arrayPtr->elements.at(index);
//                     currPtr = std::next(it) == path.partsEnd()? elementPtr : elementPtr->element;
//                     break;
//                 }
//             case tempo_config::ConfigPathPartType::Key:
//                 {
//                     if (currPtr->type != tempo_config::internal::PieceType::Object)
//                         return {};
//                     auto *objectPtr = static_cast<tempo_config::internal::ObjectPiece *>(currPtr);
//                     auto key = part.getKey();
//                     auto entry = objectPtr->keyIndex.find(key);
//                     if (entry == objectPtr->keyIndex.cend())
//                         return {};
//                     auto index = entry->second;
//                     if (index < 0 || objectPtr->members.size() <= index)
//                         return {};
//                     auto *memberPtr = objectPtr->members.at(index);
//                     currPtr = std::next(it) == path.partsEnd()? memberPtr : memberPtr->value;
//                     break;
//                 }
//             default:
//                 return {};
//         }
//     }
//
//     //
//     if (parentPtr == nullptr)
//         return {};
//
//     switch (parentPtr->type) {
//         case tempo_config::internal::PieceType::Array:
//             {
//                 auto *arrayPtr = static_cast<tempo_config::internal::ArrayPiece *>(parentPtr);
//                 auto *elementPtr = static_cast<tempo_config::internal::ElementPiece *>(currPtr);
//                 if (elementPtr->index == arrayPtr->separators.size()) {
//                     if (!arrayPtr->separators.empty()) {
//                         return std::pair(currPtr, arrayPtr->separators.back());
//                     }
//                     return std::pair(currPtr, nullptr);
//                 }
//                 return std::pair(currPtr, arrayPtr->separators.at(elementPtr->index));
//             }
//         case tempo_config::internal::PieceType::Object:
//             {
//                 auto *objectPtr = static_cast<tempo_config::internal::ObjectPiece *>(parentPtr);
//                 auto *memberPtr = static_cast<tempo_config::internal::MemberPiece *>(currPtr);
//                 if (memberPtr->index == objectPtr->separators.size()) {
//                     if (!objectPtr->separators.empty()) {
//                         return std::pair(currPtr, objectPtr->separators.back());
//                     }
//                     return std::pair(currPtr, nullptr);
//                 }
//                 return std::pair(currPtr, objectPtr->separators.at(memberPtr->index));
//             }
//     default:
//         return {};
//     }
// }

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
    auto *parentPtr = find_parent_piece(path, m_store->root(), lastPart);
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

// tempo_utils::Status
// tempo_config::ConfigEditor::replaceNode(const ConfigPath &path, const ConfigNode &node)
// {
//     ConfigPathPart lastPart;
//     auto parentPtr = find_parent_piece(path, m_root, lastPart);
//     if (parentPtr == nullptr)
//         return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
//             "failed to replace node; parent node not found at path");
//
//     return {};
// }
//
// tempo_utils::Status
// tempo_config::ConfigEditor::removeNode(const ConfigPath &path)
// {
//     // find the piece at the specified path
//     internal::JsonPiece *targetPtr;
//     internal::JsonPiece *contextPtr;
//     if (path.isRoot()) {
//         targetPtr = m_root;
//         contextPtr = nullptr;
//     } else {
//         auto result = find_piece(path, m_root);
//         targetPtr = result.first;
//         contextPtr = result.second;
//     }
//     if (targetPtr == nullptr)
//         return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
//             "failed to remove node; node not found at path");
//
//     // keep track of the piece prior to the target (which could be nullptr)
//     auto *prevPtr = targetPtr->prev;
//
//     // keep track of the last piece contained within the subtree of the target
//     internal::JsonPiece *lastPtr;
//     if (targetPtr->last != nullptr) {
//         lastPtr = targetPtr->last;
//     } else {
//         lastPtr = targetPtr;
//     }
//
//     // starting at targetPtr, delete each piece up to and including lastPtr. when we have
//     // exited the loop nextPtr will point to either the piece after lastPtr or nullptr.
//     internal::JsonPiece *nextPtr;
//     for (;;) {
//         nextPtr = targetPtr->next;
//         delete targetPtr;
//         if (targetPtr == lastPtr)
//             break;
//         targetPtr = nextPtr;
//         TU_ASSERT (targetPtr != nullptr);
//     }
//
//     // if prevPtr points to a valid piece then link it to nextPtr, otherwise if prevPtr
//     // is nullptr then make nextPtr the new head.
//     if (prevPtr != nullptr) {
//         prevPtr->next = nextPtr;
//         if (nextPtr != nullptr) {
//             nextPtr->prev = prevPtr;
//         }
//     } else {
//         auto **head = m_store->headPtr();
//         *head = nextPtr;
//         (*head)->prev = nullptr;
//     }
//
//     // if nextPtr is nullptr then update head to point to prevPtr
//     if (nextPtr == nullptr) {
//         auto **tail = m_store->tailPtr();
//         *tail = prevPtr;
//     }
//
//     //
//     if (contextPtr != nullptr) {
//         TU_ASSERT (contextPtr->last == nullptr);
//         auto *contextPrev = contextPtr->prev;
//         auto *contextNext = contextPtr->next;
//         if (contextPrev) {
//             contextPrev->next = contextNext;
//         }
//         if (contextNext) {
//             contextNext->prev = contextPrev;
//         }
//         delete contextPtr;
//     }
//
//     return {};
// }

tempo_utils::Status
tempo_config::ConfigEditor::replaceNode(const ConfigPath &path, const ConfigNode &node)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant, "unimplemented");
}

tempo_utils::Status
tempo_config::ConfigEditor::removeNode(const ConfigPath &path)
{
    ConfigPathPart lastPart;
    auto *parentPtr = find_parent_piece(path, m_store->root(), lastPart);
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