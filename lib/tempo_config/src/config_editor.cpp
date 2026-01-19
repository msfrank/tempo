
#include <ANTLRInputStream.h>

#include <tempo_config/config_editor.h>
#include <tempo_config/config_result.h>
#include <tempo_config/internal/json_piece.h>
#include <tempo_utils/log_console.h>

#include "ConfigLexer.h"

tempo_config::ConfigEditor::ConfigEditor()
    : m_root(nullptr),
      m_head(nullptr),
      m_tail(nullptr)
{
}

tempo_config::ConfigEditor::~ConfigEditor()
{
    reset();
}

tempo_utils::Status
tempo_config::ConfigEditor::parse(std::string_view utf8)
{
    m_utf8 = utf8;

    antlr4::ANTLRInputStream input(m_utf8.data(), m_utf8.size());
    tcf1::ConfigLexer lexer(&input);
    antlr4::BufferedTokenStream tokens(&lexer);
    tokens.fill();

    for (int i = 0; i < tokens.size(); i++) {
        auto *t = tokens.get(i);
        TU_ASSERT (t != nullptr);

        internal::Token token;
        token.type = t->getType();
        if (token.type == -1)
            break;
        token.channel = t->getChannel();
        token.offset = t->getStartIndex();
        token.span = (t->getStopIndex() + 1) - token.offset;

        auto text = t->getText();

        TU_CONSOLE_ERR << "token channel=" << token.channel
            << " type=" << token.type
            << " offset=" << token.offset
            << " span=" << token.span
            << " text='" << text << "'";

        switch (token.type) {

            //
            case tcf1::ConfigLexer::TrueKeyword:
            case tcf1::ConfigLexer::FalseKeyword:
            case tcf1::ConfigLexer::NullKeyword:
            case tcf1::ConfigLexer::HexInteger:
            case tcf1::ConfigLexer::DecimalInteger:
            case tcf1::ConfigLexer::OctalInteger:
            case tcf1::ConfigLexer::HexFloat:
            case tcf1::ConfigLexer::DecimalScientificFloat:
            case tcf1::ConfigLexer::DecimalFixedFloat:
            case tcf1::ConfigLexer::CharLiteral:
            case tcf1::ConfigLexer::StringLiteral:
            case tcf1::ConfigLexer::UrlLiteral:
                {
                    auto piece = std::make_unique<internal::LiteralPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (push(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::BracketOpen:
                {
                    auto piece = std::make_unique<internal::ArrayPiece>(token);
                    TU_RETURN_IF_NOT_OK (push(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::BracketClose:
                {
                    auto piece = std::make_unique<internal::BracketClosePiece>(token);
                    TU_RETURN_IF_NOT_OK (pop(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::CurlyOpen:
                {
                    auto piece = std::make_unique<internal::ObjectPiece>(token);
                    TU_RETURN_IF_NOT_OK (push(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::CurlyClose:
                {
                    auto piece = std::make_unique<internal::CurlyClosePiece>(token);
                    TU_RETURN_IF_NOT_OK (pop(std::move(piece)));
                    break;
                }

            // operator pieces
            case tcf1::ConfigLexer::ColonOperator:
                {
                    auto piece = std::make_unique<internal::ColonPiece>(token);
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            case tcf1::ConfigLexer::CommaOperator:
                {
                    auto piece = std::make_unique<internal::CommaPiece>(token);
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            // comment pieces
            case tcf1::ConfigLexer::SingleLineComment:
            case tcf1::ConfigLexer::MultiLineComment:
                {
                    auto piece = std::make_unique<internal::CommentPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            // whitespace piece
            case tcf1::ConfigLexer::EXPRWS:
                {
                    auto piece = std::make_unique<internal::WhitespacePiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            default:
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "invalid lexer token");
        }
    }

    return {};
}

static tempo_config::internal::JsonPiece *
find_parent_piece(
    const tempo_config::ConfigPath &path,
    tempo_config::internal::JsonPiece *root,
    tempo_config::ConfigPathPart &last)
{
    if (path.isRoot())
        return nullptr;

    std::vector parts(path.partsBegin(), path.partsEnd());
    last = parts.back();
    parts.pop_back();

    tempo_config::internal::JsonPiece *currPtr = root;
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
                auto entry = objectPtr->members.find(key);
                if (entry == objectPtr->members.cend())
                    return nullptr;
                auto *memberPtr = entry->second;
                currPtr = memberPtr->value;
                break;
            }
        default:
            return nullptr;
        }
    }

    return currPtr;
}

/**
 * Find the piece at the specified path. If the piece exists then return a pair containing the
 * pointer to the piece and an optional pointer to the additional context piece.
 *
 * @param path
 * @param root
 * @return
 */
static std::pair<
    tempo_config::internal::JsonPiece *,
    tempo_config::internal::JsonPiece *>
find_piece(const tempo_config::ConfigPath &path, tempo_config::internal::JsonPiece *root)
{
    tempo_config::internal::JsonPiece *currPtr = root;
    tempo_config::internal::JsonPiece *parentPtr = nullptr;

    for (auto it = path.partsBegin(); it != path.partsEnd(); ++it) {
        if (currPtr == nullptr)
            return {};
        const auto &part = *it;
        switch (part.getType()) {
            case tempo_config::ConfigPathPartType::Index:
                {
                    if (currPtr->type != tempo_config::internal::PieceType::Array)
                        return {};
                    auto *arrayPtr = static_cast<tempo_config::internal::ArrayPiece *>(currPtr);
                    auto index = part.getIndex();
                    if (index < 0 || arrayPtr->elements.size() <= index)
                        return {};
                    parentPtr = currPtr;
                    auto *elementPtr = arrayPtr->elements.at(index);
                    currPtr = std::next(it) == path.partsEnd()? elementPtr : elementPtr->element;
                    break;
                }
            case tempo_config::ConfigPathPartType::Key:
                {
                    if (currPtr->type != tempo_config::internal::PieceType::Object)
                        return {};
                    auto *objectPtr = static_cast<tempo_config::internal::ObjectPiece *>(currPtr);
                    auto key = part.getKey();
                    auto entry = objectPtr->members.find(key);
                    if (entry == objectPtr->members.cend())
                        return {};
                    parentPtr = currPtr;
                    auto *memberPtr = entry->second;
                    currPtr = std::next(it) == path.partsEnd()? memberPtr : memberPtr->value;
                    break;
                }
            default:
                return {};
        }
    }

    //
    if (parentPtr == nullptr)
        return {};

    switch (parentPtr->type) {
        case tempo_config::internal::PieceType::Array:
            {
                auto *arrayPtr = static_cast<tempo_config::internal::ArrayPiece *>(parentPtr);
                auto *elementPtr = static_cast<tempo_config::internal::ElementPiece *>(currPtr);
                if (elementPtr->index == arrayPtr->separators.size()) {
                    if (!arrayPtr->separators.empty()) {
                        return std::pair(currPtr, arrayPtr->separators.back());
                    }
                    return std::pair(currPtr, nullptr);
                }
                return std::pair(currPtr, arrayPtr->separators.at(elementPtr->index));
            }
        case tempo_config::internal::PieceType::Object:
            {
                auto *objectPtr = static_cast<tempo_config::internal::ObjectPiece *>(parentPtr);
                auto *memberPtr = static_cast<tempo_config::internal::MemberPiece *>(currPtr);
                if (memberPtr->index == objectPtr->separators.size()) {
                    if (!objectPtr->separators.empty()) {
                        return std::pair(currPtr, objectPtr->separators.back());
                    }
                    return std::pair(currPtr, nullptr);
                }
                return std::pair(currPtr, objectPtr->separators.at(memberPtr->index));
            }
    default:
        return {};
    }
}

tempo_utils::Status
tempo_config::ConfigEditor::replaceNode(const ConfigPath &path, const ConfigNode &node)
{
    ConfigPathPart lastPart;
    auto parentPtr = find_parent_piece(path, m_root, lastPart);
    if (parentPtr == nullptr)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "failed to replace node; node not found at path");

    return {};
}

tempo_utils::Status
tempo_config::ConfigEditor::removeNode(const ConfigPath &path)
{
    // find the piece at the specified path
    internal::JsonPiece *targetPtr;
    internal::JsonPiece *contextPtr;
    if (path.isRoot()) {
        targetPtr = m_root;
        contextPtr = nullptr;
    } else {
        auto result = find_piece(path, m_root);
        targetPtr = result.first;
        contextPtr = result.second;
    }
    if (targetPtr == nullptr)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "failed to remove node; node not found at path");

    // keep track of the piece prior to the target (which could be nullptr)
    auto *prevPtr = targetPtr->prev;

    // keep track of the last piece contained within the subtree of the target
    internal::JsonPiece *lastPtr;
    if (targetPtr->last != nullptr) {
        lastPtr = targetPtr->last;
    } else {
        lastPtr = targetPtr;
    }

    // starting at targetPtr, delete each piece up to and including lastPtr. when we have
    // exited the loop nextPtr will point to either the piece after lastPtr or nullptr.
    internal::JsonPiece *nextPtr;
    for (;;) {
        nextPtr = targetPtr->next;
        delete targetPtr;
        if (targetPtr == lastPtr)
            break;
        targetPtr = nextPtr;
        TU_ASSERT (targetPtr != nullptr);
    }

    // if prevPtr points to a valid piece then link it to nextPtr, otherwise if prevPtr
    // is nullptr then make nextPtr the new head.
    if (prevPtr != nullptr) {
        prevPtr->next = nextPtr;
        if (nextPtr != nullptr) {
            nextPtr->prev = prevPtr;
        }
    } else {
        m_head = nextPtr;
        m_head->prev = nullptr;
    }

    // if nextPtr is nullptr then update head to point to prevPtr
    if (nextPtr == nullptr) {
        m_tail = prevPtr;
    }

    //
    if (contextPtr != nullptr) {
        TU_ASSERT (contextPtr->last == nullptr);
        auto *contextPrev = contextPtr->prev;
        auto *contextNext = contextPtr->next;
        if (contextPrev) {
            contextPrev->next = contextNext;
        }
        if (contextNext) {
            contextNext->prev = contextPrev;
        }
        delete contextPtr;
    }

    return {};
}

void
tempo_config::ConfigEditor::reset()
{
    m_utf8.clear();
    while (!m_stack.empty()) {
        m_stack.pop();
    }

    m_root = nullptr;
    m_tail = nullptr;

    while (m_head != nullptr) {
        auto *nextPtr = m_head->next;
        delete m_head;
        m_head = nextPtr;
    }
}

std::string
tempo_config::ConfigEditor::toString() const
{
    //tu_uint32 line = 0;
    //tu_uint32 positionInLine = 0;
    std::string out;

    for (auto *currPtr = m_head; currPtr != nullptr; currPtr = currPtr->next) {
        absl::StrAppend(&out, currPtr->value);
    }

    return out;
}

tempo_utils::Status
tempo_config::ConfigEditor::append(std::unique_ptr<internal::OperatorPiece> &&piece)
{
    if (m_stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "invalid lexer token");
    auto curr = m_stack.top();
    //auto *piecePtr = internal::append_piece(std::move(piece), &m_head, &m_tail);
    return curr->append(std::move(piece), &m_head, &m_tail);
}

tempo_utils::Status
tempo_config::ConfigEditor::append(std::unique_ptr<internal::CommentPiece> &&piece)
{
    if (!m_stack.empty()) {
        auto curr = m_stack.top();
        return curr->append(std::move(piece), &m_head, &m_tail);
    }
    internal::append_piece(std::move(piece), &m_head, &m_tail);
    return {};
}

tempo_utils::Status
tempo_config::ConfigEditor::append(std::unique_ptr<internal::WhitespacePiece> &&piece)
{
    if (!m_stack.empty()) {
        auto curr = m_stack.top();
        return curr->append(std::move(piece), &m_head, &m_tail);
    }
    internal::append_piece(std::move(piece), &m_head, &m_tail);
    return {};
}

tempo_utils::Status
tempo_config::ConfigEditor::push(std::unique_ptr<internal::LiteralPiece> &&piece)
{
    if (m_root == nullptr) {
        TU_ASSERT (m_stack.empty());
        auto *piecePtr = internal::append_piece(std::move(piece), &m_head, &m_tail);
        m_stack.push(piecePtr);
        m_root = piecePtr;
        return {};
    }
    auto currPtr = m_stack.top();
    return currPtr->append(std::move(piece), &m_head, &m_tail);
}

tempo_utils::Status
tempo_config::ConfigEditor::push(std::unique_ptr<internal::ArrayPiece> &&piece)
{
    if (m_root == nullptr) {
        TU_ASSERT (m_stack.empty());
        auto *piecePtr = internal::append_piece(std::move(piece), &m_head, &m_tail);
        m_stack.push(piecePtr);
        m_root = piecePtr;
        return {};
    }
    auto currPtr = m_stack.top();
    return currPtr->append(std::move(piece), &m_head, &m_tail);
}

tempo_utils::Status
tempo_config::ConfigEditor::push(std::unique_ptr<internal::ObjectPiece> &&piece)
{
    if (m_root == nullptr) {
        TU_ASSERT (m_stack.empty());
        auto *piecePtr = internal::append_piece(std::move(piece), &m_head, &m_tail);
        m_stack.push(piecePtr);
        m_root = piecePtr;
        return {};
    }
    auto currPtr = m_stack.top();
    return currPtr->append(std::move(piece), &m_head, &m_tail);
}

tempo_utils::Status
tempo_config::ConfigEditor::pop(std::unique_ptr<internal::OperatorPiece> &&piece)
{
    if (m_stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "stack is empty");
    auto currPtr = m_stack.top();

    switch (piece->token.type) {
        case tcf1::ConfigLexer::BracketClose:
            {
                if (currPtr->type != internal::PieceType::Array)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                break;
            }
        case tcf1::ConfigLexer::CurlyClose:
            {
                if (currPtr->type != internal::PieceType::Object)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                break;
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }

    m_stack.pop();
    currPtr->last = internal::append_piece(std::move(piece), &m_head, &m_tail);
    return {};
}